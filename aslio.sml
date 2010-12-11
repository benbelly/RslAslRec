local
    fun intFromString s : int = Option.valOf (Int.fromString s);
    fun intListFromStringList ss : int list = map intFromString ss;
    fun splitLine line : string list = String.tokens Char.isSpace line;
	
    fun getDatFiles dirName : string list =
	List.filter (fn f => String.isSuffix ".dat" f) (getFiles dirName);
    fun getSignSubDirs dirName : string list =
	List.filter (fn d => String.isPrefix "sign" d) (getSubDirs dirName);
    fun getSentenceSubDirs dirName : string list =
	List.filter (fn d=> String.isPrefix "Sentence" d) (getSubDirs dirName);
in

structure AslIO =
struct

datatype feature = Face of int * int * int * int
		 | Dominant of int list
		 | Weak of int list
		 | Missing;

(* Frame number and features (Face, Dominant, Weak) *)
datatype frame = Frame of int * feature * feature * feature;
(* Directory, gloss, frames *)
datatype gloss = Gloss of string * string * frame list;
(* Directory, sentence, glosses *)
datatype sentence = Sentence of string * string * gloss list;
(* Root dir and list of sentences *)
datatype root = Root of string * sentence list;

exception BadData of string;

(*
 * Functions to create features
 *)
fun lineToFace numlist : feature =
    let val breakList = fn (tx::ty::bx::by::[]) => Face(tx, ty, bx, by)
			 | _ => raise (BadData "Incorrect number of coordinate for Face")
    in
	breakList (intListFromStringList numlist)
    end;
fun lineToDominant numlist : feature = Dominant(intListFromStringList numlist);
fun lineToWeak numlist : feature = Weak(intListFromStringList numlist);

(*
 * Functions to create Frames
 * frame files look like S01_1_0282.dat
 *)

fun getFrameNum filename : int =
    let val tokens = String.tokens (fn c => c = #"_") filename
    in
	Option.valOf (Int.fromString (List.nth(tokens, 2)))
    end;

fun inFrame ("FACE:"::ns) (Frame(num, Missing, dominant, weak)) : frame =
    Frame(num, lineToFace ns, dominant, weak)
  | inFrame ("DOMINANT:"::ns) (Frame(num, face, Missing, weak)) =
    Frame(num, face, lineToDominant ns, weak)
  | inFrame ("WEAK:"::ns) (Frame(num, face, dominant, Missing)) =
    Frame(num, face, dominant, lineToWeak ns)
  | inFrame [] frm = frm (* possible blank lines *)
  | inFrame _ _ = raise BadData "No match for feature line";

fun linesToFrames frm strm =
    let val line = TextIO.inputLine strm
    in
	if (Option.isSome line) then
	    linesToFrames (inFrame (splitLine (Option.valOf line)) frm) strm
	else
	    frm
    end;

fun frameFromFile file : frame =
    let val num = getFrameNum file
	val instream = TextIO.openIn file
	val frames = linesToFrames (Frame(num, Missing, Missing, Missing)) instream
	val _ = TextIO.closeIn instream
    in
	frames
    end;

fun validFrame (Frame(_, Face(_,_,_,_), Dominant(ds), Weak(ws))) : bool =
    length ds mod 2 = 0 andalso length ws mod 2 = 0
  | validFrame _ = false;

(*
 * Functions to create glosses
 * gloss dirs look like sign_lipread_0001
 * datatype gloss = Gloss of string * string * frame list;
 *)
fun signGloss dirName : string = List.nth(String.tokens (fn c => c = #"_") dirName, 1);

fun glossForDir dirName : gloss =
    inDir dirName (fn () => Gloss(dirName, signGloss dirName,
                                  map frameFromFile (getDatFiles ".")));

(*
 * Functions to create sentences
 * Sentence directories look like Sentence 1.1 lipread can i
 *)
fun sentenceGlosses dirName : string = 
    String.concatWith " " (List.drop(String.tokens Char.isSpace dirName, 2));

fun sentenceForDir dirName : sentence =
    inDir dirName (fn () => Sentence( dirName, sentenceGlosses dirName,
                                      map glossForDir (getSignSubDirs ".")));

(*
 * Functions to create the Root directory
 *)
fun rootForDir dirName : root =
    inDir dirName (fn () => Root( dirName, map sentenceForDir (getSentenceSubDirs ".")));

fun imagesForFrame (Frame(_,_,dom,weak)) : int list * int list =
    let val d2l = fn (Dominant(ps)) => ps
        |            (Weak(ps)) => ps
        |            _ => []
    in
      (d2l dom, d2l weak)
    end;

fun imagesForGloss (Gloss(_,_,fs)) : (int list * int list) list = map imagesForFrame fs;
fun imagesForSentence (Sentence(_, _, gs)) : (int list * int list) list =
    foldr op@ [] (map imagesForGloss gs);
fun imagesForRoot (Root(_, ss)) : (int list * int list) list =
    foldr op@ [] (map imagesForSentence ss);


fun sentenceNumber dirName : real =
    Option.valOf (Real.fromString (hd (tl (String.tokens Char.isSpace dirName))));

end; (* struct end *)

end
