
val levenshteinC = _import "levenshteinC" : char vector * int * char vector * int -> int;
val startWord = "**START**"
val endWord = "**END**"
val meWord = "**ME**"

fun atEnd { word = "**END**" } = (NONE, true)
  | atEnd _ = (NONE, false)

fun isWord "**START**" = false
  | isWord "**END**" = false
  | isWord "**ME**" = false
  | isWord _ = true

fun isWords ws = List.filter isWord ws

fun stripInterval (w,_,_) = w
fun stripIntervals ws = List.map stripInterval ws

fun wordStr ws = String.concatWith " " (isWords (stripIntervals ws))

fun wordsThru e ws =
  let
    val (thru, _) = List.partition (fn (_, b, _) => b < e) ws
  in
    thru
  end

fun isNextGroup is =
  let
    val {truth = firstTruth, alpha = firstAlpha, level = firstLevel} = hd is
  in
    (NONE, fn {truth, alpha, level} => (NONE, truth = firstTruth andalso level = firstLevel
                                              andalso Real.==(alpha, firstAlpha)))
  end

fun isFirstTruth is =
  let
    val {truth = firstTruth} = hd is
  in
    (NONE, fn ({truth}) => (NONE, truth = firstTruth))
  end

fun isFirstLevel is =
  let
    val {level = firstLevel} = hd is
  in
    (NONE, fn {level} => (NONE, firstLevel = level))
  end

fun isFirstAlpha is =
  let
    val {alpha = firstAlpha} = hd is
  in
    (NONE, fn {alpha} => (NONE, Real.==(firstAlpha, alpha)))
  end

fun bestScore is =
  let
    val scores = List.map (fn {score} => score) is
    val best = List.foldl Real.min Real.maxFinite scores
  in
    (NONE, fn {score} => (NONE, Real.==(score, best)))
  end

fun bestError is =
  let
    val errors = List.map (fn {editError} => editError) is
    val best = List.foldl Real.min Real.maxFinite errors
  in
    (NONE, fn {editError} => (NONE, Real.==(editError, best)))
  end

fun isTruth ( i : Interp.r ) =
  let
    val { prevs = (ps, _), truth, word, ... } = i
    val pwords = List.map stripInterval ps
    val trueSent = List.map stripInterval truth
    val testSent = List.filter isWord (rev (word::pwords))
  in
    (NONE, trueSent = testSent)
  end

fun isTruthStr truth test =
  let
    val truStr = wordStr truth
    val testStr = wordStr (rev test)
  in
    String.isPrefix testStr truStr
  end

fun truthStrCounts truth tests =
  let
    val trueStrs = List.filter (isTruthStr truth) tests
    val cnt = List.length tests
  in
    (List.length trueStrs, cnt)
  end

fun isTruthSoFarTest truth test =
  let
    val (_,_,intervalEnd) = hd test
    val trueSoFar = wordsThru intervalEnd truth
    val pwords = List.map stripInterval test
    val trueSent = List.map stripInterval trueSoFar
    val testSent = List.filter isWord (rev pwords)
  in
    trueSent = testSent
  end

fun truthCounts truth tests =
  let
    val total = length tests
    val truthCount = length (List.filter (isTruthSoFarTest truth) tests)
  in
    (truthCount, total)
  end

fun isTruthSoFar (i: Interp.r) =
  let
    val { interval = (b, e), prevs = (ps, _), truth, word, ... } = i
    val wordInterval = (word,b,e)
    val testWords = wordInterval::ps
  in
    (NONE, isTruthSoFarTest truth testWords)
  end

fun atAlpha(a) = fn { alpha } => (NONE, Real.==(a , alpha))

fun prForAlpha(a) = fn _ => "For alpha " ^ (Real.toString a) ^ ": "

fun prCount (is : Interp.r list) = "Count: " ^ (Int.toString(length is) ^ "\n")

fun prMaxError is =
  let
    val errs = List.map (fn {editError} => editError) is
    val max = List.foldl Real.max 0.0 errs
  in
    "Max error = " ^ (Real.toString max) ^ "\n"
  end

fun prMinError is =
  let
    val errs = List.map (fn {editError} => editError) is
    val min = List.foldl Real.min Real.maxFinite errs
  in
    "Min error = " ^ (Real.toString min) ^ "\n"
  end

fun prAvgError is =
  let
    val errs = List.map (fn {editError} => editError) is
    val sum = List.foldl op+ 0.0 errs
    val count = List.length errs
    val avg = sum / Real.fromInt(count)
  in
    "Average error = " ^ (Real.toString avg) ^ "\n"
  end

fun averageError es =
  let
    val sum = List.foldl op+ 0.0 es
    val count = List.length es
    val avg = sum / (Real.fromInt(count))
  in
    avg
  end

fun averageErrorPartitioned truth is =
  let
    val wordsAndErrors = List.map (fn {word, prevs = (ps, _), interval = (tb, te), editError, ...} => ((word, tb, te)::ps, editError)) is
    val (truths, falses) = List.partition (fn (test, _) => isTruthStr truth test) wordsAndErrors
    val trueErrors = List.map (fn (_, editError) => editError) truths
    val falseErrors = List.map (fn (_, editError) => editError) falses
    val trueAvg = averageError trueErrors
    val falseAvg = averageError falseErrors
  in
    (trueAvg, falseAvg)
  end

fun trueSentence truth =
  let
    val words = List.map stripInterval truth
    val sentence = String.concatWith " " words
  in
    sentence
  end

fun anyTrues truth tests =
  let
    val trueSent = List.map stripInterval truth
    val testsStripped = List.map stripIntervals tests
    val trimSent = fn sent => List.filter isWord (rev (sent))
    val trimmed = List.map trimSent testsStripped
  in
    List.exists (fn (s) => s = trueSent) trimmed
  end

(*
 *interp:
 *    wordMap : (string * int) list
 *    alpha : real
 *    truth : (string * int * int) list
 *    range : int * int
 *    level : int
 *    word : string
 *    score : real
 *    interval : int * int
 *    prevs : (string * int * int) list * real
 *    editDistance: int
 *    editError: real
 *)
 (* Alpha, Truth String, Level, Max Error, Min Error, Avg Error, NumTruth, NumInterps, pctTruth,
    Average truth error, average non-truth error, isFullMatch, -- word, atEnd, score, isBest, isTruth *)
fun prGroupReport (is : Interp.r list) =
  let
    val i = hd is
    val {truth, alpha, level, ...} = i
    val truthSentence = "\"" ^ (trueSentence truth) ^ "\""

    val scores = List.map (fn ({score,...}) => score) is
    val bestScore = List.foldl Real.min 0.0 scores

    val errors = List.map (fn ({editError, ...} : Interp.r) => editError) is
    val avgerr = averageError errors
    val maxerr = List.foldl Real.max 0.0 errors
    val minerr = List.foldl Real.min Real.maxFinite errors
    val (averageTruthError, averageNonTruthError) = averageErrorPartitioned truth is

    val allTests = List.map (fn {word, interval = (tb,te), prevs=(ps,_),...} => (word,tb,te)::ps) is
    val (truthCount, total) = truthStrCounts truth allTests
    val pctTruth = Real.fromInt(truthCount) / Real.fromInt(total)

    val isFullMatch = anyTrues truth allTests
    val groupStr = String.concatWith "\t" [ Real.toString alpha, truthSentence,
                                           Int.toString level, Real.toString maxerr,
                                           Real.toString minerr, Real.toString avgerr,
                                           Int.toString truthCount, Int.toString total,
                                           Real.toString pctTruth, Real.toString averageTruthError,
                                           Real.toString averageNonTruthError,
                                           Bool.toString isFullMatch ]
    val reportLine = fn ({word, score, interval = (b,e), prevs = (ps,_), editError, ...} : Interp.r) =>
        let
          val testSentence = (word,b,e)::ps
          val atEnd = "**END**" = word
          val isBest = Real.<= (score, bestScore)
          val isTruth = isTruthSoFarTest truth testSentence
          val ifields = String.concatWith "\t" [ groupStr, "\"" ^ word ^ "\"",
                                                Bool.toString atEnd, Real.toString score,
                                                Bool.toString isBest, 
                                                Bool.toString isTruth ]
        in
          ifields
        end
  in
    (*(String.concatWith "\n" (List.map reportLine is)) ^ "\n"*)
    groupStr ^ "\n"
  end

fun prOneTruth is =
  let
    val {truth} = hd is
    val words = List.map stripInterval truth
    val sentence = String.concatWith " " words
  in
    sentence ^ "\n"
  end

fun prOneLevel is =
  let
    val {level} = hd is
  in
    (Int.toString level) ^ "\n"
  end

fun prOneAlpha is =
  let
    val {alpha} = hd is
  in
    (Real.toString alpha) ^ "\n"
  end
  
fun prOneGroup is =
  let
    val {truth, alpha, level} = hd is
    val words = List.map stripInterval truth
    val sentence = String.concatWith " " words
  in
    "Truth: " ^ sentence ^
    "\nAlpha: " ^ (Real.toString alpha) ^
    "\nLevel: " ^ (Int.toString level) ^ "\n"
  end

fun rangeVec (b,e) =
  let
    val start = b - 1
    val len = (e - start) + 1
  in
    Vector.tabulate (len, fn i => start + i)
  end

fun indexOf wordMap w =
  let
    val (_, idx) = valOf(List.find (fn (word,_) => w = word) wordMap)
  in
    idx
  end

fun frameToIndex wordMap _ (0, _) = indexOf wordMap startWord
  | frameToIndex wordMap sais (idx, frame) =
    case List.find (fn (wrd, b, e) => b <= frame andalso frame <= e) sais of
         NONE => indexOf wordMap meWord | SOME(w, _, _) => indexOf wordMap w

fun sentenceString wordMap testFrames sentence =
  let
    val wordIdxs = Vector.mapi (frameToIndex wordMap sentence) testFrames
  in
    (Vector.foldr op:: [] wordIdxs)
  end

fun levenshtein tests truths =
  let
    val testChars = List.map Char.chr tests
    val truthChars = List.map Char.chr truths
  in
    levenshteinC( Vector.fromList(testChars), List.length testChars,
                  Vector.fromList(truthChars), List.length truthChars )
  end

fun levenshteinError i =
let
    val { wordMap, truth, word, prevs = (ps, _), range = (b, _),
          interval = (ib, e), editError } = i
    val testWords = rev ((word, ib, e)::ps)
    val foundSent = sentenceString wordMap (rangeVec (b, e)) testWords
    val truthSent = sentenceString wordMap (rangeVec (b, e)) truth
    val distance = levenshtein foundSent truthSent
    val err = (Real.fromInt(distance) / Real.fromInt(length truthSent))
in
  (NONE, [(NONE, {editError = err})])
end

fun levenshteinDistance i =
let
    val { wordMap, truth, word, prevs = (ps, _), range = (b, _),
          interval = (ib, e), editDistance } = i
    val testWords = rev ((word, ib, e)::ps)
    val foundSent = sentenceString wordMap (rangeVec (b, e)) testWords
    val truthSent = sentenceString wordMap (rangeVec (b, e)) truth
    val distance = levenshtein foundSent truthSent
in
  (NONE, [(NONE, {editDistance = distance})])
end

fun prLevenshteinDistance ( i : Interp.r ) =
  let
    val { wordMap, truth, word, prevs = (ps, _), range = (b, _), interval = (ib, e), ... } = i
    val testWords = rev ((word, ib, e)::ps)
    val foundSent = sentenceString wordMap (rangeVec (b, e)) testWords
    val truthSent = sentenceString wordMap (rangeVec (b, e)) truth
    val distance = levenshtein foundSent truthSent
    val pct = (Real.fromInt(distance) / Real.fromInt(length truthSent)) * 100.0
  in
    (Int.toString distance) ^ " = " ^
    (Real.toString pct) ^ "% of " ^
    (Int.toString (length truthSent)) ^ "\n"
  end

fun makewordmap is =
  let
    val nextNum = fn ws => (List.foldl (fn ((_,n),p) => Int.max(n,p)) 0 ws) + 1
    val addFoundWord = fn(ws,w,NONE) => (w, nextNum ws)::ws
                        |(ws,_,_)=> ws
    val addWord = fn(ws,w)=> addFoundWord(ws,w,(List.find (fn (word,_) => word = w) ws))
    val addWordList = fn(ws, lst) => List.foldl (fn((w, _, _),acc) => addWord(acc,w)) ws lst
    val addInterp = fn({wordMap = _, truth, word, prevs = (ps,_)},ws) =>
      let
        val wws = addWord(ws,word)
        val withTruth = addWordList(wws, truth)
        val withPrevs = addWordList(withTruth, ps)
      in
        withPrevs
      end
    val wordMap = List.foldl addInterp [] is
  in
    (NONE, fn _ => (NONE, [(NONE, {wordMap = wordMap})]))
  end

fun prWordMap {wordMap} =
  let
    val strs = List.map (fn (w, i) => w ^ " " ^ (Int.toString i)) wordMap
  in
    (String.concatWith " " strs) ^ "\n"
  end

