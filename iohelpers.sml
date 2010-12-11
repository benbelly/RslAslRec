local
    (* Reads a directory stream and returns a list of items matching test *)
    fun getItemFromStream soFar str test : string list =
	let val name = OS.FileSys.readDir str
	    val toList = fn n => if (test n) then n::[] else [];
	in
	    if (Option.isSome name) then
		getItemFromStream (soFar @ (toList (Option.valOf name))) str test
	    else soFar
	end;
in
(* Perform some action in a directory *)
fun inDir dir (action : unit -> 'a) : 'a = 
    let val oldDir = OS.FileSys.getDir()
	val _ = OS.FileSys.chDir dir
	val result = action()
	val _ = OS.FileSys.chDir oldDir
    in
	result
    end;

(* Get the subdirectories of a directory stream *)
fun getSubDirsOfStream str : string list = getItemFromStream [] str OS.FileSys.isDir;
(* Get the files of a directory stream *)
fun getFilesOfStream str : string list =
    getItemFromStream [] str (fn f => not(OS.FileSys.isDir f));

(* Get the files in a directory *)
fun getFiles dirName : string list = inDir dirName (fn () => getFilesOfStream (OS.FileSys.openDir dirName));
(* Get the subdirectories of a directory *)
fun getSubDirs dirName : string list = inDir dirName (fn () => getSubDirsOfStream (OS.FileSys.openDir dirName));

end
