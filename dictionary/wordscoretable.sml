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

type key = (int * int)
type value = real

fun keyCompare (kl : key, kr: key) =
  let
    val (wordl, endl) = kl
    val (wordr, endr) = kr
  in
    case Int.compare(wordl, wordr) of
         LESS => LESS
       | GREATER => GREATER
       | EQUAL => Int.compare(endl, endr)
  end

structure WordScoreTable = BinaryMapFn( struct type ord_key = key
                                               val compare = keyCompare
                                        end)
