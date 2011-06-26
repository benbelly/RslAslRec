
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

fun stripInterval (w,_,_) = w
fun wordsThru e ws =
  let
    val (thru, _) = List.partition (fn (_, b, _) => b < e) ws
  in
    thru
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

fun isTruthSoFar (i: Interp.r) =
  let
    val { interval = (_, e), prevs = (ps, _), truth, word, ... } = i
    val trueSoFar = wordsThru e truth
    val pwords = List.map stripInterval ps
    val trueSent = List.map stripInterval trueSoFar
    val testSent = List.filter isWord (rev (word::pwords))
  in
    (NONE, trueSent = testSent)
  end

fun atAlpha(a) = fn { alpha } => (NONE, Real.==(a , alpha))
fun prForAlpha(a) = fn _ => "For alpha " ^ (Real.toString a) ^ ": "

fun prCount (is : Interp.r list) = (Int.toString(length is) ^ "\n")

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

