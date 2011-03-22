
local
  (* C++ functions for running the algorithm *)
  val initAslAlgC       = _import "InitAslAlgC" : string vector * int vector * int vector * int
                                                      -> unit;
  val findKeyframesC    = _import "findKeyframesC" : int * real array -> unit;
  val numKeyframesC     = _import "numKeyframesC" : unit -> int;
  val getKeyframeIdsC   = _import "getKeyframeIdsC" : int array -> unit;
  val makeInitSDsC      = _import "initialSDsC" : int vector * char vector * int *
                                                  int vector * char vector * int * 
                                                  int * int * int -> unit;
  val skinmaskSDC       = _import "skinmaskSDC" : int * char vector *
                                                  char vector *
                                                  int * int * int -> unit;
  val edgeAndMaskSDC    = _import "edgeAndMaskSDC" : int * char vector *
                                                     int * int * int -> unit;
  val removeSmallComponentsC = _import "removeSmallComponentsC" : int * char vector *
                                                     int * int * int -> unit;
  val extractBoundaryC  = _import "extractBoundaryC" : int * char vector *
                                                        int * int * int -> unit;
  val numFramesC        = _import "numFramesC" : int -> int;
  val findHandsC        = _import "findHandsC" : unit -> unit;
  val getNumberOfSignsC = _import "getNumberOfSignsC" : unit -> int;
  val getSignLengthC    = _import "getSignLengthC" : int -> int;
  val getSignC          = _import "getSignC" : int * char array -> unit;
  val distanceC         = _import "distanceC" : string * int * int * int -> real;

  val numHandsC         = _import "numHandsC" : int * int * int * char vector -> int;
  val distancesC        = _import "distancesC" : real array * int * int * int * int vector *
                                                 int * char vector -> unit;
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

  fun makeInitSDs ids grays keyIds keys w h t =
    let
      val idvec = Vector.fromList ids
      val grayVec = Vector.concat grays
      val grayLen = List.length grays
      val keyIdVec = Vector.fromList keyIds
      val keyVec = Vector.concat keys
      val keyLen = List.length keys
    in
      makeInitSDsC( idvec, grayVec, grayLen, keyIdVec, keyVec, keyLen, w, h, t )
    end

  fun skinmaskSD id sd skin w h t =
    let
      val _ = skinmaskSDC( id, sd, skin, w, h, t )
    in
      Cvsl.getImage 3 id
    end

  fun edgeAndMaskSD frameId sd w h t =
    let
      val _ = edgeAndMaskSDC(frameId, sd, w, h, t)
    in
      Cvsl.getImage 3 frameId
    end

  fun removeSmallsFromSD frameId sd w h t =
    let
      val _ = removeSmallComponentsC(frameId, sd, w, h, t)
    in
      Cvsl.getImage 3 frameId
    end

  fun extractBoundaryImage frameId sd w h t =
    let
      val _ = extractBoundaryC( frameId, sd, w, h, t)
    in
      Cvsl.getImage 4 frameId
    end

  fun distances truth diff w h t =
    let
      val handCount = numHandsC( w, h, t, diff )
      val dArray = Array.array(handCount, ~1.0)
      val _ = distancesC( dArray, w, h, Vector.length truth, truth, t, diff )
    in
      Array.vector dArray
    end
end
