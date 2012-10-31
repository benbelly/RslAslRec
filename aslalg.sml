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

val dumpScoresC = _import "dumpScoresC" : string * int -> unit;
val loadScoresC = _import "loadScoresC" : string * int -> unit;

(* Values for communicating with C++ *)
val original = 0;
val gray = 1;
val sd = 3;
val boundary = 4;
val histogram = 5;
val training = 6;

(* * * * * * * * * * * * * * * * * * * * *
 * AslAlg functions
 * * * * * * * * * * * * * * * * * * * * *)

(* Add the train and test directory to the interpretation.
 * Initialize all the other values
 *)
fun init (_) = (NONE,
       [(NONE,
         Interp.rhcons(
           { testFrames = Vector.fromList([]),
             level = 0, word = 0,
             interval = (0,0), score = 0.0,
             prevs = ([], 0.0) } ) ) ] )

(* Initialize the system state - training and loading video *)
fun getIds (_) =
  (NONE, [(NONE, { testFrames = Cvsl.getIds 0 } ) ] )

fun highestReachedLevel (is) =
    foldl (fn({level},mx) => Int.max(level,mx)) 0 is

fun belowMaxLevel (max) = fn(is) =>
  let
    val reached = highestReachedLevel(is)
    val below = Int.<=(reached, max)
  in
    (NONE, fn _ => (NONE, below))
  end

fun scoredOut (i) =
  let
    val maxScore = 100000.0
    val { score } = i
  in
    (NONE, maxScore < score )
  end

fun atMax (is) =
  let
    val reached : int = highestReachedLevel(is)
    val makeNote = fn(l) => SOME (Note ("AtMax reached: " ^ (Int.toString reached) ^
                                        ", level: " ^ (Int.toString l)))
  in
    (NONE, (fn ({level}) => (makeNote level, level = reached)))
  end

fun notBestScore (is) =
  let
    val bestScore = List.foldl (fn ({score}, best) => Real.min(score, best)) Real.maxFinite is
  in
    (NONE, fn {score} => (NONE, Real.>(score, bestScore)))
  end

fun oldIncompleteLevel (is) =
  let
    val levels = List.map (fn ({level, interval, testFrames}) => {level = level}) is
    val reached = highestReachedLevel(levels)
    val {level = _, interval = _, testFrames} = hd is
    val lastIdx = Vector.sub(testFrames, (Vector.length testFrames) - 1)
  in
    (NONE, (fn ({level, interval=(_,e), testFrames = _}) => 
               (NONE, level < reached andalso e <> lastIdx)))
  end

fun len (is) = (Int.toString(List.length(is)) ^ "\n")

fun prlevelAt(loc) = fn (is) =>
  let
    val level = highestReachedLevel is
  in
    ("\t[" ^ loc ^ "] Level " ^ (Int.toString level) ^ " with " ^
     (Int.toString(List.length(is))) ^ " interpretations\n")
  end

fun prlevel (is) =
  let
    val level = highestReachedLevel is
  in
    ("Level " ^ (Int.toString level) ^ " with " ^ (Int.toString(List.length(is))) ^
     " interpretations\n")
  end

fun dumpScores(destFile) = fn ( _ ) =>
  let
    val _ = dumpScoresC( destFile, String.size destFile )
  in
    "Dumped\n"
  end

fun loadScores(srcFile) =
  let
    val _ = loadScoresC( srcFile, String.size srcFile )
  in
    "Scores loaded\n"
  end

fun intervalScore b e s = 
    ("begin: " ^ (Int.toString b) ^ ", end: " ^ (Int.toString e) ^
     ", score: " ^ (Real.toString s))

fun prInterval (i) =
  let
    val { interval = (b,e), score } = i
  in
    (intervalScore b e score) ^ "\n"
  end

fun scoreLevel (alpha, itemMap) = fn(i) =>
  let
    (*val _ = print "Scoring - "*)
    val { score, interval, word } = i
    val (b, e) = interval
    val item = itemOf itemMap word
    val distance = findDistance alpha b e item
  in
    (SOME (Note (sItemToString item)),
     [(NONE, { score = distance } )] )
  end

fun endOfTestFrames(e, testFrames ) =
  e = (Vector.sub(testFrames,(Vector.length testFrames) - 1))

fun notBest(is) =
  let
    val rec tryInsert = fn (tbl, i as {word, score, interval = (_,e), prevs}, NONE) =>
                                                WordScoreTable.insert(tbl, (word, e), i)
                         | (tbl, i as {word, score=nscr, interval, prevs = (_,newScr)},
                             SOME {word = ow, score=oscr, interval = oi, prevs = (_, oldScr)}) =>
                                     if (nscr + newScr) < (oscr + oldScr)
                                     then tryInsert(tbl, i, NONE)
                                     else tbl
    val addInterp = fn(i as {word, score, interval = (_,e), prevs = _}, tbl) =>
        let
          val existing = WordScoreTable.find(tbl, (word, e))
        in
          tryInsert(tbl, i, existing)
        end
    val scoreTable = List.foldl addInterp WordScoreTable.empty is
    val best = fn(w,(_,e)) => valOf( WordScoreTable.find(scoreTable, (w, e) ))
  in
    (NONE, fn i as {word, score, interval, prevs = (lst, _ : real)} =>
              let
                val {word = bw, score = _, interval = bi,
                     prevs = (blst, _ : real)} = best(word, interval)
                val isbest = interval = bi andalso lst = blst
              in
                (NONE, not( isbest ))
              end)
  end

fun atEnd({interval = (b,e), testFrames}) = (NONE, endOfTestFrames( e, testFrames))
fun notAtEnd({interval = (b,e), testFrames}) = (NONE, not( endOfTestFrames(e,testFrames)))

fun levelZero(itemMap) = fn(i) =>
  let
    val { level, word, interval, score, prevs, testFrames } = i
    val nextLvl = 0
    val startIdx = indexOf itemMap Start
    val zeroIdx = Vector.sub(testFrames, 0) - 1
    val zeroInterval = (zeroIdx, zeroIdx)
    val interpForInterval = (NONE, { level = nextLvl, word = startIdx,
                                     interval = zeroInterval,
                                     score = 0.0,
                                     prevs = ( [], 0.0 ) } )
  in
    ( NONE, [interpForInterval] )
  end

fun levelUpMunge itemMap  = fn(is) =>
  let
    val ilist = InterpSet.rvalue is
    val { testFrames, level, word = _,
          interval = _, score = _, prevs = _ } = hd ilist
    val nextlvl = level + 1
    val wordsForInterval = fn (b,e) => map (fn wrd =>
                                                 { testFrames = testFrames,
                                                   level = nextlvl, word = wrd,
                                                   interval = (b,e), score = 0.0,
                                                   prevs = ([], 0.0) } )
                                           (goodWords itemMap)
    val intervals = makeIntervals (nextlvl - 1) testFrames
    val interpList = List.foldl op@ [] (Vector.foldl op:: []
                                                     (Vector.map wordsForInterval intervals))
    val consed = List.map (fn i => (NONE, Interp.rhcons(i))) interpList
    val orig = InterpSet.fold (fn (i,l) => (NONE, i)::l) [] is
  in
    (SOME (Note (intervalsToString intervals)), orig @ consed )
  end

fun addEnd(itemMap) = fn(i) =>
  let
    val {word, prevs = ( ps, pscr : real ), score : real, interval = (b,e) } = i
    val newScore = score + pscr
    val endIdx = indexOf itemMap End
  in
    (NONE, [(NONE, { word = endIdx, interval = (e+1,e+1), score = newScore,
                     prevs = ((word, b, e)::ps, newScore ) })])
  end

fun badGrammar (itemMap, grammar) = fn (i) =>
  let
    val { word, prevs = (prevs, _) } = i
    val words = List.map (fn (w,_,_) => w) prevs
    val prevItems = List.map (itemOf itemMap) (word::words)
  in
    (NONE, not (validSequence grammar prevItems))
  end

fun updatePrevs (itemMap, grammar) = fn(is) =>
  let
    val maxLevel = highestReachedLevel
            (List.map (fn{prevs, score, interval, level, word} => {level = level}) is)
  in
    (* If this is not the last level, just return the interpretation untouched *)
    (NONE, fn({level, prevs, score, interval, word}) =>
           if level < maxLevel then (NONE, [(NONE,{ prevs = prevs })]) else
             let
               val (myStart, _) = interval
               (* No double words allowed right now. Grammar doesn't have any and this
                * eliminates the ME->ME problem *)
               val targetPrev = fn {prevs, interval=(_,ie), score, level=lvl, word=w} =>
                                    lvl = (level-1) andalso ie = (myStart-1) andalso
                                    w <> word
               val grammarOk = fn {level, prevs = (ws, _), score, interval, word} =>
                        let
                          val words = List.map (fn (w,_,_) => w) ws
                          val prevItems = List.map (itemOf itemMap) (word::words)
                        in
                          validSequence grammar prevItems
                        end
               val lastLevels = List.filter targetPrev is
               val goodInterps = List.filter grammarOk lastLevels 
               val interps = List.map (fn ({prevs = (plist, pscr : real),
                                            score = scr, interval = (pstrt, pend),
                                            level = lvl, word = wrd }) =>
                                          (NONE, { prevs = ( (wrd, pstrt, pend) :: plist,
                                                             scr + pscr ) } ) )
                                      goodInterps
             in
               (NONE, interps)
             end)
  end

