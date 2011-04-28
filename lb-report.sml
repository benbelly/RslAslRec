
val levenshteinC = _import "levenshteinC" : char vector * int * char vector * int -> int;

  (* tests is a list of test indices, truths is a list of truth indices *)
fun levenshtein tests truths =
  let
    val testChars = List.map Char.chr tests
    val truthChars = List.map Char.chr truths
  in
    levenshteinC( Vector.fromList(testChars), List.length testChars,
                  Vector.fromList(truthChars), List.length truthChars )
  end

fun idsForTruth itemMap truthWords =
  let
    val truthItems = List.map stringTosItem truthWords
  in
    List.map (indexOf itemMap) truthItems
  end

fun truthStringToWords truthString = String.tokens Char.isSpace truthString

fun sPrintLevenshteinDistance(itemMap, truth) = fn (i : Interp.r) =>
  let
    val { prevs = (ps, _), word, ... } = i
    val sentence = word::(List.map (fn (w,_,_) => w) (rev ps))
    val sentenceLen = (length sentence)
    val truthSentence = idsForTruth itemMap (truthStringToWords truth)
    val distance = levenshtein sentence truthSentence
    val sentenceString = String.concatWith " " (List.map sItemToString
                                                         (List.map (itemOf itemMap) sentence))
    val pct = (Real.fromInt(distance) / Real.fromInt(sentenceLen)) * 100.0
    val _ = print ("Truth: " ^ truth ^ "\n" ^
                   "Interp: " ^ sentenceString ^ "\n" ^
                   "Distance: " ^ (Int.toString distance) ^ "\n" ^
                   "Percent change: " ^ (Real.toString pct) ^ "\n")
  in
    ""
  end
