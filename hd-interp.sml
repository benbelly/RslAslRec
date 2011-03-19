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
fun keyframeSize is = "keyframe size is " ^
                      Int.toString(List.length (List.filter (fn i:Interp.r =>
                        let val { keyframe, ...} = i in keyframe end) is)) ^
                        "\n"

fun initInterp _ =
  (NONE, [(NONE, Interp.rhcons( { srcDir = "", frameId = ~1,
                                  frame = defaultFrame,
                                  diff = defaultFrame, skin = defaultFrame,
                                  gray = defaultFrame, boundary = defaultFrame,
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
    (SOME (FrameDiffs diffArray),
     fn ({keyframe, frameId}) =>
      (NONE,
       [(NONE,
         { keyframe = (Vector.exists (fn i => i = frameId) keyIds) } )] ))
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

    (*[SkinmaskDiff] update diff observing frameId, skin: skinmaskDiffs*)
fun skinmaskDiffs i =
  let
    val { frameId, diff = (sd, w, h, t), skin = (sk, _, _, _) } = i
  in
    (NONE, [(NONE, { diff = skinmaskSD frameId sd sk w h t })])
  end

    (*[EdgeAndMask] update diff observing frameId: edgeAndMaskDiffs*)
fun edgeAndMaskDiffs i =
  let
    val { frameId, diff = (sd, w, h, t) } = i
  in
    (NONE, [(NONE, { diff = edgeAndMaskSD frameId sd w h t })])
  end

    (*[RemoveSmallComponents] update diff observing frameId: removeSmallComponents*)
fun removeSmallComponents i =
  let
    val { frameId, diff = (sd, w, h, t) } = i
  in
    (NONE, [(NONE, { diff = removeSmallsFromSD frameId sd w h t })])
  end

    (*[BoundaryImage] update boundary observing frameId, diff: extractBoundary*)
fun extractBoundary i =
  let
    val { frameId, diff = (sd, w, h, t), boundary } = i
  in 
    (NONE, [(NONE, { boundary = extractBoundaryImage frameId sd w h t })])
  end
(* RZ: ICCV addition *)
fun sNumFramesMsg is = "  Number of Frames: " ^ Int.toString(List.length is) ^ "\n"
 
fun bIsKeyFrame {keyframe} = (NONE, keyframe)

fun uniqueImage (i : Interp.r) = 
  let
    val {frame, srcDir, diff, skin, gray, boundary, ... } = i
  in
    (NONE, [ (NONE, { srcDir = srcDir, frameId = ~1, frame = frame,
                      diff = diff, skin = skin, gray = gray,
                      boundary = boundary,
                      keyframe = false }) ])
  end

fun uniqueDiffImage (i : Interp.r) = 
  let
    val {frame, frameId, srcDir, diff, skin, boundary, gray, ... } = i
  in
    (NONE, [ (SOME(FrameId(frameId)), { srcDir = srcDir, frameId = ~1, frame = defaultFrame,
                      diff = diff, skin = defaultFrame, boundary = defaultFrame,
                      gray = defaultFrame, keyframe = false }) ])
  end

val interpToString = fn (i: Interp.r) =>
  let
    val {frameId, keyframe, ...} = i
  in
    "FrameId: " ^ Int.toString(frameId) ^ " Key? " ^ Bool.toString(keyframe) 
  end

val hcInterpToString = fn (i : Interp.t) =>
    interpToString(Interp.rvalue i)

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

fun sBoundaryDisplay i =
  let
    val {boundary} = i
    val _ = Cvsl.showImage boundary
  in
    ()
  end

fun sDisplayBoundaries is =
  let
    val _ = List.app sBoundaryDisplay is
  in
    ""
  end

(* Output attributes *)
val interpEdgeToString = fn t =>
    fn (i, elist) =>
    let
      val istring = hcInterpToString(i) ^ ":\n"
      val estring =
        List.foldl
        (fn (e', s') =>
        let
             val esrc = (DGraph.Node.id (DGraph.Edge.src e'))
             val edst = (DGraph.Node.id (DGraph.Edge.dst e'))
             val label = Trace.EdgeLabel.toString(Trace.labelOnEdge(t,e'))
        in
          s' ^ ("  (" ^ Word32.toString(esrc) ^ ", " ^ Word32.toString(edst) ^
          "): " ^ label ^ "\n")
        end
        )
        ""
        elist
       val _ = TextIO.print(istring ^ estring) 
    in
      istring ^ estring
    end

val tableEntryToString = fn (i, elist) =>
    let
      val istring = hcInterpToString(i) ^ ":\n"
      val estring = 
           List.foldl
           (fn ((label, _, (e : DGraph.Edge.t)), s')  =>
            let
              val esrc = (DGraph.Node.id (DGraph.Edge.src e))
              val edst = (DGraph.Node.id (DGraph.Edge.dst e))
            in
                s' ^ ("  (" ^ Word32.toString(esrc) ^", " ^ Word32.toString(edst) ^ "): "
                ^ label ^ "\n")
            end
           )
           ""
           elist
       (* Hack: side-effect *)
       val _ = TextIO.print(istring ^ estring) 
    in
      istring ^ estring
    end

val hprintTraceTable = fn (is, ah, trace) =>
    let
      val _ = TextIO.print("---------------------------------------\n")
      val _ = TextIO.print(" Attribute Table ")
      val _ = TextIO.print("size: " ^ Int.toString((HashTable.tablesize ah)) ^
      "\n")
      val it = Trace.getInterpEdgeTable (trace)
       val _ = TextIO.print(" Trace edge table size: " ^
         (Int.toString(HashTable.tablesize(it))) ^ "\n")  
      val _ = TextIO.print(" Passed Interpretations: " ^ (Int.toString(List.length is))
        ^ "\n")
      val _ = TextIO.print("---------------------------------------\n")

      val _ = HashTable.mapi (interpEdgeToString(trace)) it 
    in
      ""
    end

val sHprintUserTable = fn (is, ah, trace ) =>
    let
      val _ = TextIO.print("---------------------------------------\n")
      val _ = TextIO.print(" Attribute Table ")
      val _ = TextIO.print("size: " ^ Int.toString((HashTable.tablesize ah)) ^
      "\n")
      val it = Trace.getInterpEdgeTable (trace)
       val _ = TextIO.print(" Trace edge table size: " ^
         (Int.toString(HashTable.tablesize(it))) ^ "\n")  
      val _ = TextIO.print(" Interpretations: " ^ (Int.toString(List.length is))
        ^ "\n")
      val _ = TextIO.print("---------------------------------------\n")

      val _ = TextIO.print("\nUSER TABLE:\n")
      val _ = HashTable.mapi tableEntryToString ah
    in
      ""
    end

