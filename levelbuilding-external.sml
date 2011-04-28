
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
