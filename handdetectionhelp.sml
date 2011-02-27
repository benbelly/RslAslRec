
local

  (* C++ functions for running the algorithm *)
  val initAslAlgC       = _import "InitAslAlgC" : string vector * int vector * int vector * int
                                                      -> unit;
  val findKeyframesC    = _import "findKeyframesC" : int * real array -> unit;
  val numKeyframesC     = _import "numKeyframesC" : unit -> int;
  val getKeyframeIdsC   = _import "getKeyframeIdsC" : int array -> unit;
  val numFramesC        = _import "numFramesC" : int -> int;
  val findHandsC        = _import "findHandsC" : unit -> unit;
  val getNumberOfSignsC = _import "getNumberOfSignsC" : unit -> int;
  val getSignLengthC    = _import "getSignLengthC" : int -> int;
  val getSignC          = _import "getSignC" : int * char array -> unit;
  val distanceC         = _import "distanceC" : string * int * int * int -> real;
in

  fun init (frames, sizes, nums, numnums) =
    initAslAlgC( frames, sizes, nums, numnums )

  fun keyframeDiffs t1 =
    let
      (* Calculate keyframes and collect differences *)
      val numFrames = numFramesC( 0 )
      val diffArray = Array.array( numFrames - 1, ~1.0 )
      val _ = findKeyframesC( t1, diffArray )
    in
      Array.vector diffArray
    end

  fun keyframeIds() =
    let
      val numKeys = numKeyframesC()
      val keysIds = Array.array( numKeys, ~1 )
      val _ = getKeyframeIdsC( keysIds )
    in
      Array.vector keysIds
    end

  fun findHands() =
    let
        val _ = findHandsC();
    in
      numFramesC (3)
    end

end
