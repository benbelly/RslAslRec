local
    fun getItemFromStream soFar str test : string list =
	let val name = OS.FileSys.readDir str
	    val toList = fn n => if (test n) then n::[] else [];
	in
	    if (Option.isSome name) then
		getItemFromStream (soFar @ (toList (Option.valOf name))) str test
	    else soFar
	end;

    fun getSubDirsOfStream  str : string list = getItemFromStream [] str OS.FileSys.isDir;
    fun getFilesOfStream str : string list =
	getItemFromStream [] str (fn f => not(OS.FileSys.isDir f));
    fun getDatFilesOfStream str : string list =
	List.filter (fn f => String.isSuffix ".dat" f) (getFilesOfStream str);

    fun intFromString s : int = Option.valOf (Int.fromString s);
    fun intListFromStringList ss : int list = map intFromString ss;
    fun splitLine line : string list = String.tokens Char.isSpace line;
in
structure AslIO =
struct

fun getFiles dirName : string list = getFilesOfStream (OS.FileSys.openDir dirName);
fun getDatFiles dirName : string list = getDatFilesOfStream (OS.FileSys.openDir dirName);
fun getSubDirs dirName : string list = getSubDirsOfStream (OS.FileSys.openDir dirName);

datatype feature = Face of int * int * int * int
		 | Dominant of int list
		 | Weak of int list
		 | Missing;

fun prFeature (Face(tx, ty, bx, by)) = print ("Face: tx = " ^ (Int.toString tx) ^
					      ", ty = " ^ (Int.toString ty) ^
					      ", bx = " ^ (Int.toString bx) ^
					      ", by = " ^ (Int.toString by) ^ "\n")
  | prFeature (Dominant(xs)) = print ("Dominant hand of " ^ (Int.toString ((length xs) div 2)) ^
				      " points\n")
  | prFeature (Weak(xs)) = print ("Weak hand of " ^ (Int.toString ((length xs) div 2)) ^
				  " points\n")
  | prFeature Missing = print "Missing\n";

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
 *
 * Functions to create features
 *
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
 *
 * Functions to create Frames
 * frame files look like S01_1_0282.dat
 *
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
    in
	linesToFrames (Frame(num, Missing, Missing, Missing)) instream
    end;

(*
 *
 * Functions to create glosses
 * gloss dirs look like sign_lipread_0001
 * datatype gloss = Gloss of string * string * frame list;
 *
 *)
fun signGloss dirName : string = List.nth(String.tokens (fn c => c = #"_") dirName, 1);

fun glossForDir dirName : gloss =
    Gloss(dirName, signGloss dirName, map frameFromFile (getDatFiles dirName));

end;

end
