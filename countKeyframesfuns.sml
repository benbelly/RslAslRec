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

(*
 * All functions that deal with the RSL interpretation type
 * should be in this file. Once the interpretation type is
 * broken apart into its components, those components will
 * be passed to other functions.
 *
 * Current style is to break down the interpretation, pass
 * the data to external functions for processing, then
 * use the result to construct the return interpretation
 *
 * See getFrames
 *)
 

val defaultFrame = ( Vector.fromList( [] : char list ), ~1, ~1, ~1 )

fun len is = "Interp set length is " ^ Int.toString(List.length is) ^ "\n"

fun dInitInterp _ =
    (NONE, [(NONE, Interp.rhcons( { srcDir = "", frameId = ~1,
                                  frame = defaultFrame, t1 = ~1,
                                  keyframe = false } ))] )

fun loadDir testDir = fn _ =>
  let
    val (candidateFrames, nums) = AslIO.getSortedCandidates testDir
    val _ = init( candidateFrames, Vector.map size candidateFrames, nums,
                  Vector.length candidateFrames );
  in
    (NONE, [(NONE, {srcDir = testDir})])
  end

fun getFrames (_) =
  let 
    val frames = Vector.foldl op:: [] (Cvsl.getIds 0)
    val interps = map (fn f => (NONE, { frameId = f })) frames
  in
    (NONE, interps)
  end

fun dGetFramesImages (_) =
  let 
    val frames = Vector.foldl op:: [] (Cvsl.getIds 0)
    val interps = map (fn f => (NONE, { frameId = f, T1 = ~1, frame = Cvsl.getImage 0 f })) frames
  in
    (NONE, interps)
  end


  (* There should just be one interpretation here *)
fun dKeyframes (minT, incT) = fn is =>
  let
    val {keyframe, frameId, t1} = hd(is)
    val threshold = if t1 < 0
            then minT
            else t1 + incT
    val diffArray = keyframeDiffs threshold
    val keyIds = keyframeIds()
  in
    (NONE, fn ({keyframe, frameId, t1}) =>
      (NONE, [(NONE, { t1 = threshold, 
        keyframe = (Vector.exists (fn i => i = frameId) keyIds) } )] ))
  end

fun dDifferenceImages _ =
  let
    val _ = findHands()
  in
    (NONE, fn ({diff, frameId}) =>
                (NONE, [(NONE, {diff = Cvsl.getImage 3 frameId })]))
  end

(* RZ: ICCV addition *)
fun sNumInterpsMsg (s) =
    fn is => s ^ Int.toString(List.length is) 

fun sT1Value (is : InterpSet.r) =
    let
        val {t1, ...} = hd(is)
    in
      "Threshold: " ^ Int.toString(t1) 
    end

fun sIntStringMsg (s,i) =
    s ^ Int.toString(i) 
 
fun bIsKeyFrame {keyframe} = (NONE, keyframe)

fun uniqueImage (i : Interp.r) = 
  let
    val {frame, srcDir, ... } = i
  in
    (NONE, [ (NONE, { srcDir = srcDir, frameId = ~1, frame = frame,
                      keyframe = false }) ])
  end

val interpToString = fn (i: Interp.r) =>
  let
    val {frameId, keyframe, t1, ...} = i
  in
    "T1: " ^ Int.toString(t1) ^ " Frame: " ^ Int.toString(frameId) ^ " Keyframe: " ^ Bool.toString(keyframe) ^ "\n"
  end

  (* RZ additions for ICCV *)
fun sImageProps is = 
  let
    val head = hd is
    val {frame = (image, w, h, dt) } = head
  in
      "Image width: " ^ Int.toString(w) 
          ^ " height: " ^ Int.toString(h)
          ^ " type: " ^ Int.toString(dt)
  end

val sImgDisplay = fn i =>
  let
    val {frame = (image, w, h, dt) } = i
    val _ = Cvsl.showImage(image,w,h,dt)
  in
    ()
  end

fun cat(s1,s2) = s1 ^ s2

fun sDisplayFrames is = 
  let
    val _ = List.app
        sImgDisplay
        is
  in
    ""
  end

  
