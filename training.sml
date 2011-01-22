
(* C++ functions for training *)
val seqForGloss = _import "seqForGlossC" : int * char vector -> int ref;
val addHandsToSeq = _import "addHandsToSeqC" : int ref * int * int * int vector * int * int vector * int * int vector -> int;

(* Removed skipped sentences from the tree *)
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

  (* Pass this frame to the C++ algorithm for training *)
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

  (* Train the C++ side on this gloss *)
fun trainForGloss (AslIO.Gloss(_, word, fs)) : int list =
  let 
    val SeqRef = seqForGloss( size word, word )
  in
    map (fn f => trainForFrame SeqRef f) fs
  end;

  (* Train the C++ side on this sentence *)
fun trainForSentence (AslIO.Sentence(_, _, _, _, gs)) : unit =
  List.app (fn g => ignore(trainForGloss g)) gs

  (* Train the C++ side on the root of this tree *)
fun trainForRoot (AslIO.Root(_, ss)) : unit =
  List.app trainForSentence ss;
