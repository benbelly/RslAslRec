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

(* Test program for AslIO *)

val line = "251 1 400 200";
val tokens = String.tokens Char.isSpace line;

AslIO.prFeature (AslIO.lineToFace tokens);
AslIO.prFeature (AslIO.lineToDominant tokens);
AslIO.prFeature (AslIO.lineToWeak tokens);
AslIO.prFeature AslIO.Missing;

print "Files and dirs . . .\n";
app (fn f => print (f ^ "\n")) (AslIO.getDatFiles ".");
app (fn f => print (f ^ "\n")) (AslIO.getSubDirs ".");
print "Done.\n";
