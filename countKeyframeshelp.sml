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

    (*ML*)
    fun intToString (s) = valOf(Int.fromString(s))
    fun bNext (incT, maxT) = 
	    fn { t1 } =>  (NONE, t1 + incT < maxT + 1)

end
