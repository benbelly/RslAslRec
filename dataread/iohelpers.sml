(**
    Copyright 2011 Ben Holm

    This file is part of RslAslRec.

    RslAslRec is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RslAslRec is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RslAslRec.  If not, see <http://www.gnu.org/licenses/>.
**)

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

(* print-line *)
fun pl s = print (s ^ "\n");

end
