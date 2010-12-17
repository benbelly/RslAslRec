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
(* Directory, sentence, sentence number, instance number, glosses *)
datatype sentence = Sentence of string * string * int * int * gloss list;
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

fun linesToFrame frm strm =
    let val line = TextIO.inputLine strm
    in
	if (Option.isSome line) then
	    linesToFrame (inFrame (splitLine (Option.valOf line)) frm) strm
	else
	    frm
    end;

fun frameFromFile file : frame =
    let val num = getFrameNum file
	val instream = TextIO.openIn file
	val frames = linesToFrame (Frame(num, Missing, Missing, Missing)) instream
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
(* Get the sentence number for a directory. Directory number 2.3 would have
   a sentence number of 2 *)
fun sentenceNumberForDir dirName : int =
    let
	val nums = List.nth((String.tokens Char.isSpace dirName), 2)
	val firstNum = hd (String.tokens (fn c => c = #".") nums)
    in		     
	Option.valOf (Int.fromString firstNum)
    end;

fun sentenceNum (Sentence(_,_,num,_,_)) : int = num;

(* Get the instance number for a sentence directory. Directory number 2.3 would
   have a instance number of 3 *)
fun sentenceInstanceForDir dirName : int =
    let
	val nums = List.nth((String.tokens Char.isSpace dirName), 2)
	val secondNum = List.nth(String.tokens (fn c => c = #".") nums, 2)
    in
	Option.valOf (Int.fromString secondNum)
    end;

fun sentenceInstance (Sentence(_,_,_,ins,_)) : int = ins;


fun sentenceGlosses dirName : string = 
    String.concatWith " " (List.drop(String.tokens Char.isSpace dirName, 2));

fun sentenceForDir dirName : sentence =
    inDir dirName (fn () => Sentence( dirName, sentenceGlosses dirName,
				      sentenceNumberForDir dirName,
				      sentenceInstanceForDir dirName,
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
fun imagesForSentence (Sentence(_, _, _, _, gs)) : (int list * int list) list =
    foldr op@ [] (map imagesForGloss gs);
fun imagesForRoot (Root(_, ss)) : (int list * int list) list =
    foldr op@ [] (map imagesForSentence ss);


(* Old code - may need again someday *)
(* sentence, gloss, frame_num, image *)
(*
fun firstImage (AslIO.Root(_, ss)) : string * string * int * (int list * int list) =
  let
    val s = hd ss
    val sentence = fn (AslIO.Sentence(s, _, _, _, _)) => s
    val gloss = fn (AslIO.Sentence(_, _, _, _, (AslIO.Gloss(_,g,_))::gs)) => g
		 | _ => "gloss error"
    val fnum = fn (AslIO.Sentence(_, _, _, _, (AslIO.Gloss(_,_,(AslIO.Frame(n,_,_,_)::fs)))::gs)) => n
		| _ => ~1
    val fimg = fn (AslIO.Sentence(_, _, _, _, (AslIO.Gloss(_,_,f::fs))::gs)) => f
		| _ => AslIO.Frame(~1,AslIO.Missing, AslIO.Missing, AslIO.Missing)
  in
    (sentence s, gloss s, fnum s, AslIO.imagesForFrame (fimg s))
  end;
*)

end; (* struct end *)

end