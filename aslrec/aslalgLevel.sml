
local
val numberOfSequencesC = _import "numberOfSequencesC" : string * int -> int;
val getSequencesC = _import "getSequencesC" : string * int * (int ref) array -> unit;
val distanceC = _import "distanceC" : int ref * int * int -> real;
val getMaxScoreC = _import "getMaxScoreC" : unit -> real;
in


(* start is the starting index, testFrames is a vector of frameIds *)
fun makeIntervals start testFrames maxI =
  let
    val frames = VectorSlice.slice( testFrames, start, NONE )
    val len = VectorSlice.length frames
    val mi = maxI + 1
    val slice = fn(v,i,sz) => if (i + sz > len) then VectorSlice.subslice(v, i, NONE)
                              else VectorSlice.subslice(v, i, SOME sz)
    val matchSlice = fn (bIdx, b) => VectorSlice.map (fn(e) => (b,e))
                                                     (slice(frames,bIdx,mi))
    val pairVec = VectorSlice.mapi matchSlice frames
  in
    Vector.concat (Vector.foldr op:: [] pairVec)
  end

fun addToPrevious word levelPrevs levelScores =
  let
    val addScore = fn (score, ( words, pscore )) => (word::words, score + pscore)
  in
    ListPair.map addScore (levelScores, levelPrevs)
  end

end
