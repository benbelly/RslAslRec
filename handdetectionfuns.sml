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

fun initInterp _ =
  (NONE, [(NONE, Interp.rhcons( { srcDir = "", frameId = ~1,
                                  frame = defaultFrame,
                                  diff = defaultFrame, skin = defaultFrame,
                                  gray = defaultFrame,
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

fun getFramesImages (_) =
  let 
    val frames = Vector.foldl op:: [] (Cvsl.getIds 0)
    val interps = map (fn f => (NONE, { frameId = f, frame = Cvsl.getImage 0 f })) frames
  in
    (NONE, interps)
  end

fun skinMasks { frameId, skin } =
  (NONE, [(NONE, {skin = Cvsl.getImage 2 frameId })])

fun grayScales { frameId, gray } =
  (NONE, [(NONE, {gray = Cvsl.getImage 1 frameId })])

fun keyframes t1 = fn  _ =>
  let
    val diffArray = keyframeDiffs t1
    val keyIds = keyframeIds()
  in
    (SOME (FrameDiffs diffArray), fn ({keyframe, frameId}) =>
      (NONE, [(NONE, { keyframe = (Vector.exists (fn i => i = frameId) keyIds) } )] ))
  end

fun initialDiffImages is =
  let
    val toId = fn {keyframe, frameId, gray, diff} => frameId
    val toSize = fn {keyframe, frameId, gray = (_, w, h, t), diff } => (w, h, t)
    val toGray = fn {keyframe, frameId, gray = (gImg, _, _, _), diff} => gImg
    val isKey = fn {keyframe, frameId, gray, diff} => keyframe
    val grays = List.map toGray is
    val ids = List.map toId is
    val keys = List.map toGray (List.filter isKey is)
    val keyIds = List.map toId (List.filter isKey is)
    val (w,h,t) = toSize (hd(is))
    val _ = makeInitSDs ids grays keyIds keys w h t
  in
    (NONE, fn({keyframe, frameId, gray, diff}) =>
                (NONE, [(NONE, {diff = Cvsl.getImage 3 frameId})]))
  end

fun dDifferenceImages _ =
  let
    val _ = findHands()
  in
    (NONE, fn ({diff, frameId}) =>
                (NONE, [(NONE, {diff = Cvsl.getImage 3 frameId })]))
  end

(* RZ: ICCV addition *)
fun sNumFramesMsg is = "  Number of Frames: " ^ Int.toString(List.length is) ^ "\n"
 
fun bIsKeyFrame {keyframe} = (NONE, keyframe)

fun uniqueImage (i : Interp.r) = 
  let
    val {frame, srcDir, diff, skin, gray, ... } = i
  in
    (NONE, [ (NONE, { srcDir = srcDir, frameId = ~1, frame = frame,
                      diff = diff, skin = skin, gray = gray,
                      keyframe = false }) ])
  end

val interpToString = fn (i: Interp.r) =>
  let
    val {frameId, keyframe, ...} = i
  in
    "Frame: " ^ Int.toString(frameId) ^ " Key? " ^ Bool.toString(keyframe) 
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

fun sDisplayFrames is = 
  let
    val _ = List.app
        sImgDisplay
        is
  in
    ""
  end

fun sDiffDisplay i =
  let
    val {diff} = i
    val _ = Cvsl.showImage diff
  in
    ()
  end

fun sDisplayDifferences is =
  let
    val _ = List.app sDiffDisplay is
  in
    ""
  end
  
