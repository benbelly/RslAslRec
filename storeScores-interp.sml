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


fun signFromDir dir =
  let
    val lastDir = List.last (String.tokens (fn c => c = #"/") dir)
    val parts = String.tokens (fn c => c = #"_") lastDir
  in
    List.nth(parts, 1)
  end

fun truthInterval is =
  let
    val (begins, ends) = ListPair.unzip (map (fn {interval = i} => i) is)
    (* Add one b/c it's subtracted when we level 0 *)
    val minStart = (foldl Int.min (valOf Int.maxInt) begins) + 1
    val maxEnd = foldl Int.max 0 ends
    val isFullSign = fn (b,e) =>
      let 
        (*
         *val _ = print ("Comparing " ^ (Int.toString b) ^ " to minimum " ^
         *               (Int.toString minStart) ^ "\n\tand " ^ (Int.toString e) ^
         *               " to maximum " ^ (Int.toString maxEnd) ^ "\n")
         *)
        val result = b=minStart andalso e=maxEnd
        (*
         *val _ = print ("\tResult = " ^ (Bool.toString result) ^ "\n")
         *)
      in
        result
      end
  in
    (NONE, (fn {interval} => (NONE, isFullSign interval)))
  end

fun writeInterp (itemMap, testDir) = fn i =>
  let
    val testWord = signFromDir testDir
    val {word, interval = (b,e), score} = i
    val modelWord = sItemToString (itemOf itemMap word)
  in
    (testWord ^ " " ^ modelWord ^ " " ^
     (Int.toString b) ^ " " ^ (Int.toString e) ^ " " ^
     (Real.toString score) ^ "\n")
  end
