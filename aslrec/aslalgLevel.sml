
local
val numberOfSequencesC = _import "numberOfSequencesC" : string * int -> int;
val getSequencesC = _import "getSequencesC" : string * int * (int ref) array -> unit;
val distanceC = _import "distanceC" : int ref * int * int -> real;
val getMaxScoreC = _import "getMaxScoreC" : unit -> real;
in

  fun intervalsToString intervals =
    let
      val i2s = fn (b,e) => "(" ^ (Int.toString b) ^ ", " ^ (Int.toString e) ^ ")"
      val istrs = Vector.map i2s intervals
      val istrsList = Vector.foldl op:: [] istrs
    in
      String.concatWith " " istrsList
    end

  (* start is the starting index, testFrames is a vector of frameIds *)
  fun makeIntervals start testFrames =
    let
      val frames = VectorSlice.slice( testFrames, start, NONE )
      val len = VectorSlice.length frames
      val mi = (Vector.length testFrames) + 1
      val slice = fn(v,i,sz) => if (i + sz > len) then VectorSlice.subslice(v, i, NONE)
                                else VectorSlice.subslice(v, i, SOME sz)
      val matchSlice = fn (bIdx, b) => VectorSlice.map (fn(e) => (b,e))
                                                       (slice(frames,bIdx,mi))
      val pairVec = VectorSlice.mapi matchSlice frames
      val result = Vector.concat (Vector.foldr op:: [] pairVec)
    in
      result
    end

  fun makeIntervalsFromEnd frame testFrames =
    let
      val found = (Vector.findi (fn(_,fId) => fId = frame) testFrames)
      val (idx, _) = case found of NONE => (~1,~1)
                                 | SOME (idx, id) => (idx, id)
    in
      if idx < 0 then Vector.fromList([]) else makeIntervals idx testFrames
    end
    
end (* scope *)
