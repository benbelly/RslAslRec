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


fun spaceCat (a, b) = a ^ " " ^ b
fun spaceList l = List.foldl spaceCat "" l
fun commaCat (a, b) = a ^ ", " ^ b
fun commaList l = List.foldl commaCat "" l
fun intStr i = Int.toString(i)
fun realStr r = Real.toString(r)

fun intervalStr (b,e) = "(" ^ (intStr b) ^ ", " ^ (intStr e) ^ ")"

fun i2s itemMap = fn i =>
let
(*
 *interp:
 *    testFrames: int vector
 *    level : int
 *    word : int
 *    score : real
 *    interval : int * int
 *    prevs : int list * real
 *)
  val { testFrames, level,
        word, score,
        interval, prevs = (ps, pscr) } = i
  val prevWords = List.map (fn (w,_,_) => w) ps
  val wordStr = fn wordIdx => sItemToString (itemOf itemMap wordIdx)
  val prevStr = fn (ps, s) => "[ " ^ (commaList (List.map wordStr ps)) ^ " ] @" ^
                              (realStr s) ^ "\n"
 in
   "Level: " ^ (intStr level) ^ "\n" ^
   "Word: " ^ (wordStr word) ^ "\n" ^
   "Score: " ^ (realStr score) ^ "\n" ^
   "Interval: " ^ (intervalStr interval) ^ "\n" ^
   "Prevs: " ^ (prevStr (prevWords, pscr)) ^ "\n"
end
