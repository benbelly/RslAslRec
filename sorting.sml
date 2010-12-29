(* ML does not provide a sort implementation. Rather than write and debug my
* own, I borrowed from Wikipedia:
* http://en.wikipedia.org/wiki/Standard_ML#Mergesort
*)

local
fun split ns = let
  fun loop (x::y::zs, xs, ys) = loop (zs, x::xs, y::ys)
    | loop (x::[], xs, ys) = (x::xs, ys)
    | loop ([], xs, ys) = (xs, ys)
  in
    loop (List.rev ns, [], [])
  end

(* Merge two ordered lists using the order lt.  Pre: the given lists xs and ys
 * must already be ordered per lt.  Runs in O(n) time, where n = |xs| + |ys|. *)
fun merge lt (xs, ys) = let
  fun loop (out, left as x::xs, right as y::ys) =
              if lt (x, y) then loop (x::out, xs, right)
              else loop (y::out, left, ys)
    | loop (out, x::xs, []) = loop (x::out, xs, [])
    | loop (out, [], y::ys) = loop (y::out, [], ys)
    | loop (out, [], []) = List.rev out
  in
    loop ([], xs, ys)
  end

in
 (* Sort a list in according to the given ordering operation lt.
  * Runs in O(n log n) time, where n = |xs|.
  *)
 fun mergesort lt xs = let
   val merge' = merge lt
   fun ms [] = []
     | ms [x] = [x]
     | ms xs = let
         val (left, right) = split xs
         in
           merge' (ms left, ms right)
         end
   in
     ms xs
   end
 end
