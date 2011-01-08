(*
 *namespace frame_types {
 *    enum {
 *        original = 0,
 *        gray = 1,
 *        skin = 2,
 *        sd = 3,
 *        boundary = 4,
 *        histogram = 5,
 *        training = 6
 *    };
 *}
 *)

val init = _import "InitAslAlgC" : string vector * int vector * int vector * int -> unit;
val findHands = _import "findHandsC" : unit -> unit;
val seqForGloss = _import "seqForGlossC" : int * char vector -> int ref;
val addHandsToSeq = _import "addHandsToSeqC" : int ref * int * int * int vector * int * int vector * int * int vector -> int;

fun cleanedRoot (AslIO.Root(d,ss)) skips : AslIO.root =
    let
	val skipSentence = fn snum => List.exists (fn skip => snum = skip) skips
	val sentenceAndNumbers = List.filter (fn (s,n) => not(skipSentence n))
					     (ListPair.zip(ss, (map AslIO.sentenceNum ss)))
	val (sentences, _) = ListPair.unzip sentenceAndNumbers
    in
	AslIO.Root(d,sentences)
    end;

(* Create a Root for the traning set and a Root for the candidate set *)
fun splitSentences (AslIO.Root(d,ss)) candidateInstance : AslIO.root * AslIO.root =
    let
	val candidate = fn sentence => (AslIO.sentenceNum sentence) = candidateInstance
	val train = fn s => not(candidate s)
    in
	(AslIO.Root(d,List.filter train ss),
	 AslIO.Root(d, List.filter candidate ss))
    end;

fun trainForFrame SeqRef (AslIO.Frame(_, face, dom, weak)) : int =
    let
	val toList = fn (AslIO.Dominant(ds)) => ds
		      | (AslIO.Weak(ws)) => ws
		      | (AslIO.Face(tx,ty,bx,by)) => [tx,ty,bx,by]
		      | _ => []
	val faces = Vector.fromList (toList face)
	val doms = Vector.fromList (toList dom)
	val weaks = Vector.fromList (toList weak)
    in
	addHandsToSeq(SeqRef, 640, 480, faces,
		      Vector.length doms, doms,
		      Vector.length weaks, weaks)
    end;

fun trainForGloss (AslIO.Gloss(_, word, fs)) : int list =
    let 
	val SeqRef = seqForGloss( size word, word )
    in
	map (fn f => trainForFrame SeqRef f) fs
    end;

fun trainForSentence (AslIO.Sentence(_, _, _, _, gs)) : unit =
  List.app (fn g => ignore(trainForGloss g)) gs

fun trainForRoot (AslIO.Root(_, ss)) : unit =
  List.app trainForSentence ss;

fun getFrameNum file = Option.valOf(Int.fromString(List.nth(String.tokens (fn c=> c = #"_") file, 2)));

fun getSortedCandidates dir =
let
  val files = getFiles dir
  val lessThan = fn ((_,l),(_,t)) => l < t
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

val _ = aslalg();
print "Done.\n";
