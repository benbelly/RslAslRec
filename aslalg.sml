(*
 *namespace frame_types {
 *    enum {
 *        original = 0,
 *        gray = 1,
 *        skin = 2,
 *        sd = 3,
 *        boundary = 4,
 *        histogram = 5,
 *        special = 6
 *    };
 *}
 *)


val init = _import "InitAslAlgC" : char vector * int -> unit;
val findHands = _import "findHandsC" : unit -> unit;
val seqForGloss = _import "seqForGlossC" : int * char vector -> int ref;
val addHandsToSeq = _import "addHandsToSeqC" : int ref * int * int * int * int vector * int * int vector -> int;

(* addHandImage takes width, height, hand 1 size (number of points), hand 1 points,
   hand 2 size, hand 2 points *)
(*val addHandsImage = _import "addHandImageC" : int * int * int * int vector * int * int vector -> int;*)

(*
 *fun saveAllHands root : int list =
 *    map (fn (i1, i2) => addHandsImage(640, 480, length i1, Vector.fromList i1,
 *                      length i2, Vector.fromList i2))
 *    (AslIO.imagesForRoot root);
 *)
    
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

fun trainForFrame SeqRef (AslIO.Frame(_, _, dom, weak)) : int =
    let
	val toList = fn (AslIO.Dominant(ds)) => ds
		      | (AslIO.Weak(ws)) => ws
		      | (_) => []
	val doms = toList dom
	val weaks = toList weak
    in
	addHandsToSeq(SeqRef, 640, 480, length doms, Vector.fromList doms,
		      length weaks, Vector.fromList weaks)
    end;

fun trainForGloss (AslIO.Gloss(_, word, fs)) : int list =
    let 
	val SeqRef = seqForGloss( size word, word )
    in
	map (fn f => trainForFrame SeqRef f) fs
    end;

fun trainForSentence (AslIO.Sentence(_, _, _, _, gs)) : unit =
  List.app (fn g => ignore(trainForGloss g)) gs;

fun trainForRoot (AslIO.Root(_, ss)) : unit =
  List.app trainForSentence ss;

fun aslalg () =
  let
    val filename = "mov/S003.MOV";
    val dataDir = "/home/ben/Documents/school/USF-ASL-Data-Set-v2";
    val root = AslIO.rootForDir dataDir;
    val imgs = AslIO.imagesForRoot root
    (* val skipSentences = [1, 18, 19, 25]
    val candidate = 5 (* Test sentence instance 5 *)
    val cleaned = cleanedRoot root skipSentences
    val (trainingS, candS) = splitSentences cleaned candidate *)
  in
    init( filename, (size filename) );
    (* findHands(); *)
    Cvsl.saveAllImages "cvsl_out/train" "png" 6
  end;

val _ = aslalg();
print "Done.\n";
