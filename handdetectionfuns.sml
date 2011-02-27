


fun len is = "Interp set length is " ^ Int.toString(List.length is) ^ "\n"

fun initInterp _ =
  (NONE, [(NONE, Interp.rhcons( { srcDir = "", frame = ~1, diff = ~1, keyframe =
  false, img = ("",1,~1,~1) } ))] )

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
    val interps = map (fn f => (NONE, { frame = f })) frames
  in
    (NONE, interps)
  end

fun getFramesImages (_) =
  let 
    val frames = Vector.foldl op:: [] (Cvsl.getIds 0)
    val interps = map (fn f => (NONE, { frame = f, img = Cvsl.getImage 0 f })) frames
  in
    (NONE, interps)
  end


  (* There should just be one interpretation here *)
fun keyframes t1 = fn  _ =>
  let
    val diffArray = keyframeDiffs t1
    val keyIds = keyframeIds()
  in
    (SOME (FrameDiffs diffArray), fn ({keyframe, frame}) =>
      (NONE, [(NONE, { keyframe = (Vector.exists (fn i => i = frame) keyIds) } )] ))
  end

fun dDifferenceImages _ =
  let
    val _ = findHands()
  in
    (NONE, fn ({diff, frame, img}) => (NONE, [(NONE, {diff = frame, 
        img = Cvsl.getImage 3 frame})]))
  end

