
(* Values for communicating with C++ *)
val original = 0;
val gray = 1;
val sd = 3;
val boundary = 4;
val histogram = 5;
val training = 6;

(* C++ functions for running the algorithm *)
val init = _import "InitAslAlgC" : string vector * int vector * int vector * int -> unit;
val findHands = _import "findHandsC" : unit -> unit;

(* * * * * * * * * * * * * * * * * * * * *
 * AslAlg functions
 * * * * * * * * * * * * * * * * * * * * *)

  (* The directory contents are returned in random order, so they need to be
   * sorted *)
fun getSortedCandidates dir =
  let
    val files = getFiles dir
    val lessThan = fn ((_,l),(_,t)) => l < t
    val getFrameNum = fn file =>
      Option.valOf(Int.fromString(List.nth(String.tokens (fn c=> c = #"_") file, 2)))
    val fileNNum = ListPair.zip(files, map getFrameNum files)
    val sorted = mergesort lessThan fileNNum
    val (nameList, numList) = ListPair.unzip sorted
    val fullNameList = map (fn n => dir ^ "/" ^ n) nameList
  in
    (Vector.fromList fullNameList, Vector.fromList numList)
  end;

fun aslalg () =
  let
    val dataDir = "/home/bholm/USF-ASL-Data-Set-v2"
    val candidateDir = "/home/bholm/USF-ASL-Data-Set-v2/Sentence 2.5 lipread cannot i"
    val (candidateFrames, nums) = getSortedCandidates candidateDir
    val root = AslIO.rootForDir dataDir
    val imgs = AslIO.imagesForRoot root
    val skipSentences = [1, 18, 19, 25]
    val candidate = 5 (* Test sentence instance 5 *)
    val cleaned = cleanedRoot root skipSentences
    val (trainingS, _) = splitSentences cleaned candidate
  in
    init( candidateFrames, Vector.map size candidateFrames, nums,
          Vector.length candidateFrames );
    print "Initialized.\n";
    findHands();
    print "Found hands\n";
    print ("Training " ^ (Int.toString(length (AslIO.imagesForRoot trainingS))) ^
           " hand pairs\n");
    trainForRoot trainingS;
    print "Trained.\n";
    (*
     Cvsl.saveAllImages "cvsl_out/training" "png" 6;
     *)
     Cvsl.saveAllImages "cvsl_out/orig" "png" 0;
     Cvsl.saveAllImages "cvsl_out/skin" "png" 2;
     Cvsl.saveAllImages "cvsl_out/sd" "png" 3;
     Cvsl.saveAllImages "cvsl_out/boundary" "png" 4
     (*
      Cvsl.displayAllImages 4
      *)
  end;

