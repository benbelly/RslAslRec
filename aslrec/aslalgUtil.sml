
(* C++ functions for running the algorithm *)
val init = _import "InitAslAlgC" : string vector * int vector * int vector * int -> unit;
val findHands = _import "findHandsC" : unit -> unit;
val getNumberOfSignsC = _import "getNumberOfSignsC" : unit -> int;
val getSignLengthC = _import "getSignLengthC" : int -> int;
val getSignC = _import "getSignC" : int * char array -> unit;

fun aslalg trainDir testDir =
  let
    val (candidateFrames, nums) = AslIO.getSortedCandidates testDir
    val root = AslIO.rootForDir trainDir
    val imgs = AslIO.imagesForRoot root
    val skipSentences = [1, 18, 19, 25]
    val candidate = 5 (* Test sentence instance 5 *)
    val cleaned = cleanedRoot root skipSentences
    val (trainingS, _) = splitSentences cleaned candidate
  in
    init( candidateFrames, Vector.map size candidateFrames, nums,
          Vector.length candidateFrames );
    findHands();
    trainForRoot trainingS
    (*Cvsl.saveAllImages "cvsl_out/orig" "png" 0;*)
  end;

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
in
  ["foo"]
end;
