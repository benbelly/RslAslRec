
(*
 *interp:
 *    testDir: string
 *    trainDir: string
 *    testFrames: int list
 *)

val interpToString = fn i => "interp";

val annotToString = fn a => "annotation";

val streamToInterp = fn stream =>
    let
      val test = valOf (TextIO.inputLine(stream))
      val train = valOf (TextIO.inputLine(stream))
      (*val emptyIntList : int list = []*)
      val testFramesHashed = Interp.testFrames_STRUCT.hcnil
      val _ = Interp.testFrames_STRUCT.:##: (7, testFramesHashed )
      val interp = Interp.hcons ( { testDir = test, trainDir = train,
                                    testFrames = testFramesHashed })
    in
      (NA, interp)
    end

