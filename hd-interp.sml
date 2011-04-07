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
                                  truehand = Vector.fromList( [] : int list ),
                                  handImage = defaultFrame, skin = defaultFrame,
                                  gray = defaultFrame, keyframe = false } ))] )

fun loadDir (testDir, trainDir) = fn _ =>
  let
    val (candidateFrames, nums) = AslIO.getSortedCandidates testDir
    val _ = init( candidateFrames, Vector.map size candidateFrames, nums,
                  Vector.length candidateFrames );
    val root = AslIO.rootForDir trainDir
    val skipSentences = [1, 18, 19, 25]
    val candidate = 5 (* Test sentence instance 5 *)
    val cleaned = cleanedRoot root skipSentences
    val (trainingS, _) = splitSentences cleaned candidate
    val _ = trainForRoot trainingS
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

fun getFramesImages testDir = fn (_) =>
  let 
    val frames = Vector.foldl op:: [] (Cvsl.getIds 0)
    val trues = AslIO.handsForDir testDir
    val ids = List.map Int.toString (List.map (fn (i,_) => i) trues)
    (*val _ = print ("["^ (String.concatWith ", " ids ) ^ "]\n")*)
    val interps = map (fn f =>
      let
        val (_, trHand) = valOf(List.find (fn (id, _) => f = id) trues)
      in
        (NONE, { frameId = f, frame = Cvsl.getImage 0 f, truehand = trHand })
      end) frames
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
    val annotation = SOME (FrameDiffs diffArray)
  in
    (annotation,
     fn ({keyframe, frameId}) =>
      (annotation,
       [(annotation,
         { keyframe = (Vector.exists (fn i => i = frameId) keyIds) } )] ))
  end

fun initialDiffImages is =
  let
    val toId = fn {keyframe, frameId, gray, handImage} => frameId
    val toSize = fn {keyframe, frameId, gray = (_, w, h, t), handImage } => (w, h, t)
    val toGray = fn {keyframe, frameId, gray = (gImg, _, _, _), handImage} => gImg
    val isKey = fn {keyframe, frameId, gray, handImage} => keyframe
    val grays = List.map toGray is
    val ids = List.map toId is
    val keys = List.map toGray (List.filter isKey is)
    val keyIds = List.map toId (List.filter isKey is)
    val (w,h,t) = toSize (hd(is))
    val _ = makeInitSDs ids grays keyIds keys w h t
  in
    (NONE, fn({keyframe, frameId, gray, handImage}) =>
                (NONE, [(NONE, {handImage = Cvsl.getImage 3 frameId})]))
  end

fun dDifferenceImages _ =
  let
    val _ = findHands()
  in
    (NONE, fn ({handImage, frameId}) =>
                (NONE, [(NONE, {handImage = Cvsl.getImage 3 frameId })]))
  end

    (*[SkinmaskDiff] update handImage observing frameId, skin: skinmaskDiffs*)
fun skinmaskDiffs i =
  let
    val { frameId, handImage = (sd, w, h, t), skin = (sk, _, _, _) } = i
  in
    (NONE, [(NONE, { handImage = skinmaskSD frameId sd sk w h t })])
  end

    (*[EdgeAndMask] update handImage observing frameId: edgeAndMaskDiffs*)
fun edgeAndMaskDiffs i =
  let
    val { frameId, handImage = (sd, w, h, t) } = i
  in
    (NONE, [(NONE, { handImage = edgeAndMaskSD frameId sd w h t })])
  end

    (*[RemoveSmallComponents] update handImage observing frameId: removeSmallComponents*)
fun removeSmallComponents i =
  let
    val { frameId, handImage = (sd, w, h, t) } = i
  in
    (NONE, [(NONE, { handImage = removeSmallsFromSD frameId sd w h t })])
  end

    (*[BoundaryImage] update boundary observing frameId, handImage: extractBoundary*)
fun extractBoundary i =
  let
    val { frameId, handImage = (sd, w, h, t) } = i
    val result = extractBoundaryImage frameId sd w h t
  in 
    (NONE, [(NONE, { handImage = result })])
  end

(*
 *interp:
 *    frameId : int
 *    truehand : int vector
 *    diff : char vector * int * int * int
 *)
fun sPrintDiffAccuracy (i : Interp.r) =
  let
    val { frameId, truehand, handImage = (diffImg, w, h, t), ... } = i
    val frameStr = Int.toString frameId
    val mDistances = distancesAndCenters truehand diffImg w h t
    val best = hd (mergesort (fn ((dl,_),(dr,_)) => Real.<(dl,dr)) mDistances)
    val (bestMDist, bestCenter) = best
    val mdStr = Real.toString bestMDist
    val cenStr = Real.toString bestCenter
    val cnt = (Int.toString (handCount diffImg w h t)) ^ "\n"
  in
    (*TextIO.print( frameStr ^ ", " ^ cnt );*)
    TextIO.print (frameStr ^ "," ^ mdStr ^ "," ^ cenStr ^ "\n");
    ""
  end

fun sPrintCenterAccuracy (i : Interp.r) =
  let
    val { frameId, truehand, handImage = (diffImg, w, h, t), ... } = i
    val frameStr = Int.toString frameId
    val mDistances = distancesAndCenters truehand diffImg w h t
    val best = hd (mergesort (fn ((_,dl),(_,dr)) => Real.<(dl,dr)) mDistances)
    val (bestMDist, bestCenter) = best
    val mdStr = Real.toString bestMDist
    val cenStr = Real.toString bestCenter
    val cnt = (Int.toString (handCount diffImg w h t)) ^ "\n"
  in
    (*TextIO.print( frameStr ^ ", " ^ cnt );*)
    TextIO.print (frameStr ^ "," ^ mdStr ^ "," ^ cenStr ^ "\n");
    ""
  end

(* RZ: ICCV addition *)
fun sNumFramesMsg is = "  Number of Frames: " ^ Int.toString(List.length is) ^ "\n"
 
fun bIsKeyFrame {keyframe} = (NONE, keyframe)

fun uniqueImage (i : Interp.r) = 
  let
    val {frame, srcDir, handImage, skin, gray, ... } = i
  in
    (NONE, [ (NONE, { srcDir = srcDir, frameId = ~1, frame = frame,
                      handImage = handImage, skin = skin, gray = gray,
                      keyframe = false }) ])
  end

fun uniqueDiffImage (i : Interp.r) = 
  let
    val {frame, frameId, srcDir, handImage, skin, gray, ... } = i
  in
    (NONE, [ (SOME(FrameId(frameId)), { srcDir = srcDir, frameId = ~1, frame = defaultFrame,
                      handImage = handImage, skin = defaultFrame,
                      truehand = Vector.fromList( [] : int list ),
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
    val {handImage} = i
    val _ = Cvsl.showImage handImage
  in
    ()
  end

fun sDisplayDifferences is =
  let
    val _ = List.app sDiffDisplay is
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

fun noteStr elist =
  let
    val toNote = fn(_,n : note option ,_) => n
    val notes = List.map toNote elist
    val toStr = fn (SOME (FrameDiffs diffs)) => "Something"
                      (*String.concatWith "," (map Real.toString (Vector.foldl op:: [] diffs))*)
                 | _ => "Nothing"
    val strs = map toStr notes
  in
    TextIO.print ((String.concatWith "\n" strs) ^ "\n");
    ""
  end

fun sHprintDiffs (is, ah, trace) =
  let
    val _ = HashTable.map noteStr ah
  in
    ""
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
      val it = Trace.getInterpEdgeTable (trace)
      val _ = TextIO.print("size: " ^ Int.toString((HashTable.tablesize ah)) ^
      "\n")
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


