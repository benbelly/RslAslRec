
local
val numberOfSequencesC = _import "numberOfSequencesC" : string * int -> int;
val getSequencesC = _import "getSequencesC" : string * int * (int ref) array -> unit;
val distanceC = _import "distanceC" : int ref * int * int -> real;
val getMaxScoreC = _import "getMaxScoreC" : unit -> real;
in

  (* start is the starting index, testFrames is a vector of frameIds *)
  fun makeIntervals start testFrames allPrevs =
    let
      val maxI = Vector.length testFrames
      val frames = VectorSlice.slice( testFrames, start, NONE )
      val len = VectorSlice.length frames
      val mi = maxI + 1
      val slice = fn(v,i,sz) => if (i + sz > len) then VectorSlice.subslice(v, i, NONE)
                                else VectorSlice.subslice(v, i, SOME sz)
      val matchSlice = fn (bIdx, b) => VectorSlice.map (fn(e) => (b,e))
                                                       (slice(frames,bIdx,mi))
      val pairVec = VectorSlice.mapi (fn (bIdx, b) => (matchSlice(bIdx, b), Vector.sub(prevs,bIdx))) frames
    in
      pairVec
    end

  (*
   *fun scoreItemsAndIntervals itemMap intervals levelPrevs =
   *  let
   *    val scores = List.filter (fn (_,[]) => false
   *                               | _ => true )
   *)

end (* scope *)
