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
  in
    (NONE, fn _ => (NONE, Int.<(reached,max)))
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

fun oldIncompleteLevel (is) =
  let
    val levels = List.map (fn ({level, interval, testFrames}) => {level = level}) is
    val reached = highestReachedLevel(levels)
    val {level, interval, testFrames} = hd is
    val lastIdx = Vector.sub(testFrames, (Vector.length testFrames) - 1)
  in
    (NONE, (fn ({level, interval=(b,e), testFrames}) => 
               (NONE, level < reached andalso e <> lastIdx)))
  end

fun len (is) = (Int.toString(List.length(is)) ^ "\n")

fun prlevel (is) =
  let
    val level = highestReachedLevel is
  in
    ("Level " ^ (Int.toString level) ^ "\n")
  end

fun dumpScores(destFile) = fn ( _ ) =>
  let
    val _ = dumpScoresC( destFile, String.size destFile )
  in
    "Dumped\n"
  end

fun loadScores(srcFile) = fn (_) =>
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

fun scoreLevel (itemMap) = fn(i) =>
  let
    (*val _ = print "Scoring - "*)
    val { score, interval, level, word } = i
    val (b, e) = interval
    val item = itemOf itemMap word
    val distance = findDistance b e item
    (*
     *val _ = print ("word: " ^ (sItemToString item) ^ ", begin: " ^ (Int.toString b) ^
     *               ", end: " ^ (Int.toString e) ^ ", distance: " ^ (Real.toString distance)
     *               ^ "\n" )
     *)
  in
    (NONE, [(NONE, { score = distance } )] )
  end

fun badGrammar (itemMap, grammar, depth) = fn (i) =>
  let
    val { word, prevs = (prevs, _) } = i
    val prevItems = List.map (itemOf itemMap) (word::prevs)
  in
    (NONE, not (validSequence grammar prevItems depth))
  end

fun endOfTestFrames(e, testFrames ) =
  e = (Vector.sub(testFrames,(Vector.length testFrames) - 1))

fun notBest(is) =
  let
    val rec tryInsert = fn (tbl, i as {word, interval = (_,e), prevs}, NONE) =>
                                                WordScoreTable.insert(tbl, (word, e), i)
                         | (tbl, i as {word, interval, prevs = (_,newScr)},
                             SOME {word = ow, interval = oi, prevs = (_, oldScr)}) =>
                                     if newScr < oldScr then tryInsert(tbl, i, NONE) else tbl
    val addInterp = fn(i as {word, interval = (b,e), prevs = (_,scr)}, tbl) =>
        let
          val existing = WordScoreTable.find(tbl, (word, e))
        in
          tryInsert(tbl, i, existing)
        end
    val scoreTable = List.foldl addInterp WordScoreTable.empty is
    val best = fn(w,(_,e)) => valOf( WordScoreTable.find(scoreTable, (w, e) ))
  in
    (NONE, fn i as {word, interval = interval as (b,e), prevs = (lst, scr)} =>
              let
                val {word = bw, interval = bi,
                     prevs = (blst, bscr)} = best(word, interval)
                val isbest = word = bw andalso interval = bi andalso lst = blst andalso
                             Real.==(scr, bscr)
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
    val zeroIdx = Vector.sub(testFrames, 0)
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
    val { testFrames, level, word,
          interval, score, prevs } = hd ilist
    val nextlvl = level + 1
    val wordsForInterval = fn (b,e) => map (fn wrd =>
                                                 { testFrames = testFrames,
                                                   level = nextlvl, word = wrd,
                                                   interval = (b,e), score = 0.0,
                                                   prevs = ([], 0.0) } )
                                           (goodWords itemMap)
    val intervals = makeIntervals nextlvl testFrames
    val interpList = List.foldl op@ [] (Vector.foldl op:: []
                                                     (Vector.map wordsForInterval intervals))
    val consed = List.map (fn i => (NONE, Interp.rhcons(i))) interpList
    val orig = InterpSet.fold (fn (i,l) => (NONE, i)::l) [] is
  in
    (NONE, orig @ consed )
  end

fun levelUp(itemMap) = fn(is) =>
  let
    val firstI = hd is
    val { level, word, interval, testFrames } = firstI
    val nextlvl = level + 1
    val startIdx = indexOf itemMap Start
    val endIdx = indexOf itemMap End
    val intervals = makeIntervals nextlvl testFrames
    val (wordIdxs, _) = ListPair.unzip itemMap
    val goodWords = List.filter (fn idx => idx <> startIdx andalso idx <> endIdx) wordIdxs
    val wordsForInterval = fn (b,e) => map (fn wrd =>
                                                (NONE, { level = nextlvl, word = wrd,
                                                         interval = (b,e) } ))
                                           goodWords
    val interpList = List.foldl op@ [] (Vector.foldl op:: []
                                                     (Vector.map wordsForInterval intervals))
  in
    (NONE, fn (i) => (NONE, if i = firstI then interpList else [] ))
  end

fun addEnd(itemMap) = fn(i) =>
  let
    val {word, prevs = ( ps, pscr : real ), score : real } = i
    val newScore = score + pscr
    val endIdx = indexOf itemMap End
  in
    (NONE, [(NONE, { word = endIdx, score = newScore,
                     prevs = (word::ps, newScore ) })])
  end

fun updatePrevs(is) =
  (NONE, fn({prevs, score, interval, level, word}) =>
           let
             val (myStart, _) = interval
             (* No double words allowed right now. Grammar doesn't have any and this
              * eliminates the ME->ME problem *)
             val targetPrev = fn {prevs, interval=(_,ie), score, level=lvl, word=w} =>
                                  lvl = (level-1) andalso ie = (myStart-1) andalso
                                  w <> word
             val validPrevs = List.filter targetPrev is
             val interps = List.map (fn ({prevs = (plist, pscr : real),
                                          score = scr, interval = _,
                                          level = lvl, word = wrd }) =>
                                        (NONE, { prevs = ( wrd :: plist, scr + pscr ) } ) )
                                    validPrevs
           in
             (NONE, interps)
           end)

