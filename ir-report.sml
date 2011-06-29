fun dirname file =
  let
    val elements = String.tokens (fn c => c = #"/") file
    val lastDropped = List.take(elements, (List.length elements)-1)
    val dir = String.concatWith "/" lastDropped
  in
    dir
  end

fun lineToTuple line =
  let
    val wrdNint = String.tokens Char.isSpace line
    val wrd = hd wrdNint
    val intStr = hd (tl wrdNint)
    val splitNums = String.tokens (fn c => c = #"-") intStr
    val nums = List.map valOf (List.map Int.fromString splitNums)
    val b = List.nth(nums, 0)
    val e = List.nth(nums, 1)
  in
    (wrd, b, e)
  end

exception BadInterpretation of string

fun alphaForFile file = valOf (Real.fromString (List.nth(
                                    String.tokens (fn c => c = #"-") file, 1)))

fun truthForFile file =
  let
    val dir = dirname file
    val full = "Full"
    val instream = TextIO.openIn file
    val readLine = fn() => TextIO.inputLine instream
    val rec truths = fn(acc,NONE)=> rev acc
                      |(acc,SOME line)=>
                          if (String.isPrefix full line)
                            then truths (acc, NONE)
                            else truths ((lineToTuple line)::acc, readLine())
  in
    truths([],(readLine()))
  end


fun alphaStr a = Real.toString a
fun strAlpha astr = valOf (Real.fromString astr)
fun levelStr l = Int.toString l
fun strLevel ls = valOf (Int.fromString ls)
fun wordStr w = w
fun strWord ws = ws
fun scoreStr s = Real.toString s
fun strScore ss = valOf (Real.fromString ss)
fun intervalStr (b,e) = "(" ^ (Int.toString b) ^ ", " ^ (Int.toString e) ^ ")"
fun editDistanceStr distance = Int.toString distance
fun editErrorStr err = Real.toString err
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

fun truthStr truth =
  let
    val _ = "[Lipread|(54,68):Can|(72,80)]"
    val pStr = fn (wrd, b, e) => (wordStr wrd) ^ "|" ^ (intervalStr (b,e))
    val pStrs = List.map pStr truth
    val listContents = String.concatWith ":" pStrs
  in
    "[" ^ listContents ^ "]"
  end

fun strTruth trth =
  let
    val lstStr = String.substring(trth, 1, (String.size trth) - 2)
    val strTuples = String.tokens (fn c => c = #":") lstStr
    val wrdIntLst = List.map (String.tokens (fn c => c = #"|")) strTuples
    val wrdTupleLst = List.map (fn wrd::tple::[] => (wrd, tple)
                                 | _ => raise BadInterpretation "invalid prevs format") wrdIntLst
    val truthList = List.map (fn (wrd, interval) =>
                                    let val (b,e) = strInterval interval in (wrd, b, e) end)
                             wrdTupleLst
  in
    truthList
  end

fun prevsStr (ps, pscr) =
  let
    val _ = "[Lipread|(54, 68):Can|(72, 80)]@3.1415927"
    val listContents = truthStr ps
  in
    listContents ^ "@" ^ (scoreStr pscr)
  end

fun strPrev pstr =
  let
    val _ = "[Lipread|(54, 68):Can|(72, 80)]@3.1415927"
    val lstNscr = String.tokens (fn c => c = #"@") pstr
    val lstStrBrackets = List.nth(lstNscr,0)
    val ps = strTruth lstStrBrackets
    val pscr = strScore (List.nth(lstNscr,1))
  in
    (ps, pscr)
  end

fun interpToString i =
  let
    val { wordMap, alpha, truth, range, level, word,
          score, interval, prevs, editDistance, editError } = i
  in
    (alphaStr alpha) ^ "\n" ^
    (truthStr truth) ^ "\n" ^
    (intervalStr range) ^ "\n" ^
    (levelStr level) ^ "\n" ^
    (wordStr word) ^ "\n" ^
    (scoreStr score) ^ "\n" ^
    (intervalStr interval) ^ "\n" ^
    (prevsStr prevs) ^ "\n" ^
    (editDistanceStr editDistance) ^ "\n" ^
    (editErrorStr editError) ^ "\n"
  end

fun streamToInterp strm =
  let
    val read = fn () => let
                          val line = valOf (TextIO.inputLine strm)
                          (*val _ = print (line)*)
                          val chomped = String.implode(List.filter (fn c => c <> #"\n") (String.explode line))
                        in
                          chomped
                        end
    val alpha = strAlpha (read())
    val truth = strTruth (read())
    val range = strInterval (read())
    val level = strLevel (read())
    val word = strWord (read())
    val score = strScore (read())
    val interval = strInterval (read())
    val prevs = strPrev (read())
  in
    SOME (NONE, { wordMap = [], alpha = alpha, truth = truth, range = range,
                  level = level, word = word, score = score, interval = interval,
                  prevs = prevs, editDistance = 0, editError = 0.0 } )
  end

