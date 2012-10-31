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

