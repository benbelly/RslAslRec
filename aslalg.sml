val numFramesC        = _import "numFramesC" : int -> int;

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
             interval = (0,0,0.0),
             prevs = ([], 0.0) } ) ) ] );

(* Initialize the system state - training and loading video *)
fun getIds (_) =
  (NONE, [(NONE, { testFrames = Cvsl.getIds 0 } ) ] )

(* Create the level 1 interpretation. *)
fun levelOne (itemMap) = fn(i) =>
  let
    val { testFrames, level, word, interval, prevs } = i
    val thisLevel = 1
    val startIdx = indexOf itemMap Start
    val intervals = makeIntervals thisLevel testFrames (Vector.length testFrames)
    val (wordIdxs, _) = ListPair.unzip itemMap
    val wordsForInterval = fn (b,e) => map (fn wrd =>
                                                (NONE, { level = thisLevel, word = wrd,
                                                         interval = (b,e,0.0),
                                                         prevs = ( [startIdx], 0.0 ) } ))
                                           wordIdxs
    val interpList = List.foldl op@ [] (Vector.foldl op:: []
                                                     (Vector.map wordsForInterval intervals))
  in
    (NONE, interpList)
  end

fun highestReachedLevel (is) =
    foldl (fn({level},mx) => Int.max(level,mx)) 0 is

fun belowMaxLevel (max) = fn(is) =>
  let
    val reached = highestReachedLevel(is)
  in
    (NONE, fn _ => (NONE, Int.<(reached,max)))
  end

fun atMax (is) =
  let
    val reached : int = highestReachedLevel(is)
  in
    (NONE, (fn ({level}) => (NONE, level = reached)))
  end

fun max (is) = (Int.toString(highestReachedLevel(is)) ^ "\n")
fun len (is) = (Int.toString(List.length(is)) ^ "\n")

fun scoreLevel (itemMap) = fn(i) =>
  let
    val { interval, level, word } = i
    val (b, e, _) = interval
    val item = itemOf itemMap word
    val distance = findDistance b e item
  in
    (NONE, [(NONE, { interval = (b, e, distance ) } )] )
  end

fun levelUp(itemMap) = fn(i) =>
  let
    val { testFrames, level, word, interval } = i
    val thisLevel = level + 1
    val (b, e, _) = interval
    val wordStr = itemOf itemMap word
    val intervals = makeIntervalsFromEnd e testFrames (Vector.length testFrames)
    val makeInterp = fn(b,e) => (NONE, { level = thisLevel, word = word,
                                         interval = (b, e, 0.0) } )
    val interps = Vector.foldl op:: [] (Vector.map makeInterp intervals)
    val origInterp = (NONE, { level = level, word = word, interval = interval })
  in
    (NONE, origInterp :: interps)
  end

fun updatePrevs(is) =
  (NONE, fn({prevs, interval, level, word}) =>
           let
             val (myStart, _, _) = interval
             val targetEnd = myStart - 1
             val targetLevel = level - 1
             val validPrevs = List.filter (fn ({prevs, interval = (_, iend, _),
                                                level = lvl, word}) =>
                                              lvl = targetLevel andalso iend = targetEnd ) is
             val interps = List.map (fn ({prevs = (plist, pscr : real),
                                          interval = (_, _, scr : real),
                                          level = lvl, word = wrd }) =>
                                        (NONE, { prevs = ( wrd :: plist, scr + pscr ) } ) )
                                    validPrevs
           in
             (NONE, interps)
           end)
