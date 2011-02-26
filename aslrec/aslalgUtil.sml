
(* C++ functions for running the algorithm *)
val init              = _import "InitAslAlgC" : string vector * int vector * int vector * int
                                                    -> unit;
val findKeyframesC    = _import "findKeyframesC" : real array -> unit;
val numFramesC        = _import "numFramesC" : int -> int;
val findHandsC        = _import "findHandsC" : unit -> unit;
val getNumberOfSignsC = _import "getNumberOfSignsC" : unit -> int;
val getSignLengthC    = _import "getSignLengthC" : int -> int;
val getSignC          = _import "getSignC" : int * char array -> unit;
val distanceC         = _import "distanceC" : string * int * int * int -> real;

fun aslalgLoad trainDir testDir =
  let
    val (candidateFrames, nums) = AslIO.getSortedCandidates testDir
    val root = AslIO.rootForDir trainDir
    val imgs = AslIO.imagesForRoot root
    val skipSentences = [1, 18, 19, 25]
    val candidate = 5 (* Test sentence instance 5 *)
    val cleaned = cleanedRoot root skipSentences
    val (trainingS, _) = splitSentences cleaned candidate
    val _ = init( candidateFrames, Vector.map size candidateFrames, nums,
                  Vector.length candidateFrames );
    val numFrames = numFramesC( 0 )
    val diffs = Array.array( numFrames, ~1.0 )
  in
    findKeyframesC( diffs );
     Cvsl.saveAllImages "cvsl_out/gray" "png" 1;
    (*
     *Cvsl.saveAllImages "cvsl_out/gray" "png" 1;
     *Cvsl.saveAllImages "cvsl_out/skin" "png" 2;
     *)
    Cvsl.saveAllImages "cvsl_out/sd" "png" 3;
    (*Cvsl.saveAllImages "cvsl_out/boundary" "png" 4;*)
    findHandsC();
    trainForRoot trainingS;
    root2grammar root
  end

fun allWords() =
  let
    val cnt = getNumberOfSignsC()
    val makeword = fn i => let
                             val len = getSignLengthC(i)
                             val strArr = Array.array(len, #"*")
                             val _ = getSignC( i, strArr )
                           in
                             String.implode (Array.foldr op:: [] strArr)
                           end
    val rec allwrds = fn (acc, 0) => acc
                       | (acc, c) => let val word = makeword c in allwrds(word::acc, c - 1) end
  in
      allwrds([], cnt - 1)
  end

fun itemIndexMap() =
  let
    val allwords = List.map (fn w => Gloss(w)) (allWords())
    val nonwords = [ Start, End, ME ]
    val allItems = nonwords @ allwords
    val indices = List.tabulate(length allItems, fn i => i)
  in
    ListPair.zip(indices, allItems)
  end

fun indexOf iMap item =
  let
    val (idx, _) = valOf (List.find (fn (i, v) => item = v) iMap)
  in
    idx
  end

fun itemOf iMap index =
  let
    val (_, item) = valOf (List.find (fn (i,v) => index = i) iMap)
  in
    item
  end

fun items iMap indexList = map (itemOf iMap) indexList
fun indices iMap itemList = map (indexOf iMap) itemList

fun findDistance _ _ Start = 0.0
  | findDistance _ _ End = 0.0
  | findDistance b e ME =
      let
        val alpha = 0.35 (* value of 'nullscore' in original *)
        val distance = Real.fromInt ((e - b) + 1) (* minimum distance of 1 *)
      in
        alpha * distance (* Equation (5) in paper *)
      end
  | findDistance b e (Gloss(word)) =
      let
        val distance = distanceC( word, String.size word, b, e )
      in
        distance
      end
