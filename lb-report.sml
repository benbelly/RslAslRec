
val levenshteinC = _import "levenshteinC" : char vector * int * char vector * int -> int;

(*
 *interp:
 *    testFrames: int vector
 *    level : int
 *    word : int
 *    score : real
 *    interval : int * int
 *    prevs : (int * int * int) list * real
 *)

exception BadInterpretation of string

fun levelStr l = Int.toString l
fun strLevel ls = valOf (Int.fromString ls)
fun wordStr w = let val imap = itemIndexMap() in sItemToString (itemOf imap w) end
fun strWrd ws = ws
fun scoreStr s = Real.toString s
fun strScore ss = valOf (Real.fromString ss)
fun intervalStr (b,e) = "(" ^ (Int.toString b) ^ ", " ^ (Int.toString e) ^ ")"
fun strInterval is =
  let
    val strs = String.tokens Char.isSpace is
    val sploded = List.map String.explode strs
    val clean = List.map (List.filter (fn c => Char.isDigit c)) sploded
    val imploded = List.map String.implode clean
    val numList = List.map valOf (List.map Int.fromString imploded)
  in
    (* There better be a first and second! *)
    (List.nth(numList, 0), List.nth(numList, 1))
  end
fun prevsStr (ps, pscr) =
  let
    val _ = "[Lipread|(54, 68):Can|(72, 80)]@3.1415927"
    val pStr = fn (wrd, b, e) => (wordStr wrd) ^ "|" ^ (intervalStr (b,e))
    val pStrs = List.map pStr ps
    val listContents = String.concatWith ":" pStrs
  in
    "[" ^ listContents ^ "]@" ^ (scoreStr pscr)
  end
fun strPrev pstr =
  let
    val _ = "[Lipread|(54, 68):Can|(72, 80)]@3.1415927"
    val lstNscr = String.tokens (fn c => c = #"@") pstr
    val lstStrBrackets = List.nth(lstNscr,0)
    val lstStr = String.substring(lstStrBrackets, 1, (String.size lstStrBrackets) - 2)
    val strTuples = String.tokens (fn c => c = #":") lstStr
    val wrdIntLst = List.map (String.tokens (fn c => c = #"|")) strTuples
    val wrdTupleLst = List.map (fn wrd::tple::[] => (wrd, tple)
                                 | _ => raise BadInterpretation "invalid prevs format") wrdIntLst
    val ps = List.map (fn (wrd, interval) =>
                            let val (b,e) = strInterval interval in (wrd, b, e) end)
                      wrdTupleLst
    val pscr = strScore (List.nth(lstNscr,1))
  in
    (ps, pscr)
  end

fun interpToString i =
  let
    val { testFrames = _, level , word, score, interval, prevs } = i
  in
    (levelStr level) ^ "\n" ^
    (wordStr word) ^ "\n" ^
    (scoreStr score) ^ "\n" ^
    (intervalStr interval) ^ "\n" ^
    (prevsStr prevs)
  end

  (* tests is a list of test indices, truths is a list of truth indices *)
fun levenshtein tests truths =
  let
    val testChars = List.map Char.chr tests
    val truthChars = List.map Char.chr truths
  in
    levenshteinC( Vector.fromList(testChars), List.length testChars,
                  Vector.fromList(truthChars), List.length truthChars )
  end

fun intervalToString (b,e) = "(" ^ (Int.toString b) ^ ", " ^ (Int.toString e) ^ ")"

fun intervalFile testDir = testDir ^ "/intervals"
fun intervalLines testDir =
  let
    val inStrm = TextIO.openIn (intervalFile testDir)
    val rec linesOf = fn (acc, NONE) => rev acc
                       | (acc, (SOME str)) => linesOf (str::acc, TextIO.inputLine inStrm)
  in
    linesOf ([], TextIO.inputLine inStrm)
  end

fun take2 l = case l of a::b::_ => (a,b) | _ => raise Fail "take2"
fun signAndInterval line =
  let
    val (word,interval) = take2 (String.tokens Char.isSpace line)
    val (b,e) = take2 (String.tokens (fn c => c = #"-") interval)
  in
    (word, valOf (Int.fromString b), valOf (Int.fromString e))
  end

fun signsAndIntervals testDir =
  let
    val lines = List.filter (fn line => not(String.isPrefix "Full" line))
                            (intervalLines testDir)
  in
    map signAndInterval lines
  end

fun indexesAndStarts itemMap testDir =
  let
    val sais = signsAndIntervals testDir
  in
    map (fn (item, b, e) => (indexOf itemMap (Gloss item), b, e)) sais
  end

fun frameToIndex itemMap _ (0, _) = indexOf itemMap Start
  | frameToIndex itemMap sais (idx, frame) =
    case List.find (fn (wrd, b, e) => b <= frame andalso frame <= e) sais of
         NONE => indexOf itemMap ME | SOME(widx, _, _) => widx

fun truthString itemMap testFrames testDir =
  let
    val wordIdxs = Vector.mapi (frameToIndex itemMap (indexesAndStarts itemMap testDir)) testFrames
    val endIdx = indexOf itemMap End
  in
    (Vector.foldr op:: [] wordIdxs)@[endIdx]
  end

fun foundString itemMap testFrames sentence =
  let
    val wordIdxs = Vector.mapi (frameToIndex itemMap sentence) testFrames
    val endIdx = indexOf itemMap End
  in
    (Vector.foldr op:: [] wordIdxs)@[endIdx]
  end

fun sPrintLevenshteinDistance(itemMap, testDir) = fn (i : Interp.r) =>
  let
    val { prevs = (ps, _), word, testFrames, ... } = i
    val sentence = foundString itemMap testFrames ps
    val intervalStrings = List.map (fn (_,b,e) => intervalToString (b,e)) (rev ps)
    val sentenceLen = (length sentence)
    val truthSentence = truthString itemMap testFrames testDir
    val distance = levenshtein sentence truthSentence
    val sentenceString = String.concatWith " " (List.map sItemToString
                                                         (List.map (itemOf itemMap) sentence))
    val pct = (Real.fromInt(distance) / Real.fromInt(sentenceLen)) * 100.0
    val _ = print ("Interp: " ^ sentenceString ^ "\n" ^
                   "Distance: " ^ (Int.toString distance) ^ "\n" ^
                   "Percent change: " ^ (Real.toString pct) ^ "\n" ^
                   (String.concatWith " " intervalStrings) ^ "\n")
  in
    ""
  end
