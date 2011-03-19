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
    val { score, interval, level, word } = i
    val (b, e) = interval
    val item = itemOf itemMap word
    val distance = findDistance b e item
    (*
     *val _ = print ("begin: " ^ (Int.toString b) ^ ", end: " ^ (Int.toString e) ^
     *               ", distance: " ^ (Real.toString distance) ^ "\n" )
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

fun notBestForEnd(is) =
  let
    val wordInterps = fn w => List.filter (fn {word, interval, prevs} => word = w) is
    val intervalsForWord = fn(w,i)=> List.filter (fn {word, interval, prevs} => i = interval)
                                                 (wordInterps w)
    val best = fn(w,i) =>
        let
          val iws = intervalsForWord(w,i)
        in
          List.foldl (fn (li, ri) =>
                let
                  val {word = lw, interval = lint, prevs = (_, lscore)} = li
                  val {word = rw, interval = rint, prevs = (_, rscore)} = ri
                in
                  if Real.<(lscore,rscore) then li else ri
                end) (hd iws) (tl iws)
        end
  in
    (NONE, fn i as {word, interval, prevs = (lst, scr)} =>
              let
                val {word = bw, interval = bi, prevs = (blst, bscr)} = best(word, interval)
                val isbest = word = bw andalso interval = bi andalso lst = blst andalso
                             Real.==(scr, bscr)
              in
                (NONE, not( isbest ))
              end)
  end

fun atEnd({interval = (b,e), testFrames}) =
  (NONE, e = (Vector.sub(testFrames,(Vector.length testFrames) - 1)))

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
    val _ = print ("Consed size: " ^ (Int.toString(List.length consed)) ^"\n")
    val _ = print ("Orig size: " ^ (Int.toString(List.length orig)) ^"\n")
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
    val endIdx = indexOf itemMap End
  in
    (NONE, [(NONE, { word = endIdx, prevs = (word::ps, score + pscr) })])
  end

fun updatePrevs(is) =
  (NONE, fn({prevs, score, interval, level, word}) =>
           let
             val (myStart, _) = interval
             val targetEnd = myStart - 1
             val targetLevel = level - 1
             val validPrevs = List.filter (fn ({prevs, interval = (_, iend),
                                                score, level = lvl, word}) =>
                                              lvl = targetLevel andalso iend = targetEnd ) is
             val interps = List.map (fn ({prevs = (plist, pscr : real),
                                          score = scr, interval = _,
                                          level = lvl, word = wrd }) =>
                                        (NONE, { prevs = ( wrd :: plist, scr + pscr ) } ) )
                                    validPrevs
           in
             (NONE, interps)
           end)
