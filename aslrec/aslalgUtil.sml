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


(* C++ functions for running the algorithm *)
val init              = _import "InitAslAlgC" : string vector * int vector * int vector * int
                                                    -> unit;
val findKeyframesC    = _import "findKeyframesC" : int * real array -> unit;
val numFramesC        = _import "numFramesC" : int -> int;
val findHandsC        = _import "findHandsC" : unit -> unit;
val getNumberOfSignsC = _import "getNumberOfSignsC" : unit -> int;
val getSignLengthC    = _import "getSignLengthC" : int -> int;
val getSignC          = _import "getSignC" : int * char array -> unit;
val distanceC         = _import "distanceC" : string * int * int * int -> real;
val addTruthTestC     = _import "AddTruthTestC" : int * int * int vector *
                                                        int * int vector -> unit;

fun aslalgLoadForTruth t1 trainDir testDir =
  let
    val (candidateFrames, nums) = AslIO.getSortedCandidates testDir
    val root = AslIO.rootForDir trainDir
    val skipSentences = [18, 19]
    val candidate = 4 (* Test sentence instance 5 *)
    val cleaned = cleanedRoot root skipSentences
    val (trainingS, _) = splitSentences cleaned candidate
    val _ = init( candidateFrames, Vector.map size candidateFrames, nums,
                  Vector.length candidateFrames );
    val numFrames = numFramesC( 0 )
    val diffs = Array.array( numFrames, ~1.0 )
    val emptyVec = Vector.fromList []
    val dirHands = AslIO.handsForDir testDir
  in
    (*Cvsl.saveAllImages "cvsl_out/boundary" "png" 4;*)
    trainForRoot trainingS;
    app (fn (fnum, domHand) => addTruthTestC( fnum, Vector.length domHand, domHand,
                                              0, emptyVec )) dirHands;
    root2grammar root
  end

fun aslalgLoad t1 trainDir testDir =
  let
    val (candidateFrames, nums) = AslIO.getSortedCandidates testDir
    val root = AslIO.rootForDir trainDir
    (*val skipSentences = [18, 19]*)
    val skipSentences = [3, 5, 6, 8, 9, 11, 12, 13, 14,
                         15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25]
    val candidate = 4 (* Test sentence instance 5 *)
    val cleaned = cleanedRoot root skipSentences
    val (trainingS, _) = splitSentences cleaned candidate
    val _ = init( candidateFrames, Vector.map size candidateFrames, nums,
                  Vector.length candidateFrames );
    val numFrames = numFramesC( 0 )
    val diffs = Array.array( numFrames, ~1.0 )
  in
    findKeyframesC( t1, diffs );
    findHandsC();
    Cvsl.saveAllImages "cvsl_out/original" "png" 0;
    Cvsl.saveAllImages "cvsl_out/gray" "png" 1;
    Cvsl.saveAllImages "cvsl_out/skin" "png" 2;
    Cvsl.saveAllImages "cvsl_out/sd" "png" 3;
    Cvsl.saveAllImages "cvsl_out/boundary" "png" 4;
    trainForRoot trainingS;
    root2grammar root
  end

fun allWords() =
  let
    val cnt = getNumberOfSignsC()
    val makeword = fn i => let
                             val len = getSignLengthC(i)
                             val strArr = Array.array(len, #"*")
                             val _ = getSignC( i, strArr )
                           in
                             String.implode (Array.foldr op:: [] strArr)
                           end
    val rec allwrds = fn (acc, 0) => acc
                       | (acc, c) => let val word = makeword c in allwrds(word::acc, c - 1) end
  in
      allwrds([], cnt - 1)
  end

fun itemIndexMap() =
  let
    val allwords = List.map (fn w => Gloss(w)) (allWords())
    val nonwords = [ Start, End, ME ]
    val allItems = nonwords @ allwords
    val indices = List.tabulate(length allItems, fn i => i)
  in
    ListPair.zip(indices, allItems)
  end

fun indexOf iMap item =
  let
    val (idx, _) = valOf (List.find (fn (i, v) => item = v) iMap)
  in
    idx
  end

fun itemOf iMap index =
  let
    val (_, item) = valOf (List.find (fn (i,v) => index = i) iMap)
  in
    item
  end

fun items iMap indexList = map (itemOf iMap) indexList
fun indices iMap itemList = map (indexOf iMap) itemList

fun findDistance _ _ _ Start = 0.0
  | findDistance _ _ _ End = 0.0
  | findDistance alpha b e ME =
      let
        val distance = Real.fromInt ((e - b) + 1) (* minimum distance of 1 *)
      in
        alpha * distance (* Equation (5) in paper *)
      end
  | findDistance _ b e (Gloss(word)) =
      let
        val distance = distanceC( word, String.size word, b, e )
      in
        distance
      end

fun goodWords(itemMap) =
  let
    val startIdx = indexOf itemMap Start
    val endIdx = indexOf itemMap End
    val (wordIdxs, _) = ListPair.unzip itemMap
  in
    List.filter (fn idx => idx <> startIdx andalso idx <> endIdx) wordIdxs
  end

