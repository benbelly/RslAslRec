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
val setDirs = fn(_) => (NONE,
       [(NONE,
         Interp.rhcons(
           { testFrames = Vector.fromList([]),
             levelNum = 0, levelWord = 0,
             levelScores = Vector.fromList([]),
             levelPrevs = Vector.fromList([]) } ) ) ] );

(* Initialize the system state - training and loading video *)
val getIds = fn() => fn(i) =>
  (NONE, [(NONE, { testFrames = Cvsl.getIds 0 } ) ] )

val highestReachedLevel = fn(is) =>
    foldl Int.max 0 (List.map (fn {levelNum} => levelNum ) is)

val belowMaxLevel = fn (max) => fn(is) =>
  let
    val reached = highestReachedLevel(is)
  in
    (NONE, fn _ => (NONE, Int.<(reached,max)))
  end

val atMax = fn(is) =>
  let
    val reached : int = highestReachedLevel(is)
  in
    (NONE, (fn ({levelNum : int}) => (NONE, levelNum = reached)))
  end

val levelZero = fn(itemMap) => fn(_) =>
  let
    val start = indexOf itemMap Start
    val num = numFramesC(0)
  in
    (NONE, [(NONE, { levelNum = 0, levelWord = start,
                     levelScores = Vector.tabulate(num, fn(_) => 0.0),
                     levelPrevs = Vector.tabulate(num, fn(_) => ([], 0.0)) })])
  end


(*[NextLevel] update levelNum, levelScores: scoreNextLevel()*)
val scoreNextLevels = fn(itemMap) => fn i =>
  let 
    val { levelNum, levelScores, levelPrevs,
          levelWord, testFrames } = i
    val nextLevel = levelNum + 1
    val intervals = makeIntervals nextLevel testFrames levelPrevs
    val scores = scoreItemsAndIntervals itemMap intervals levelPrevs
  in
    scores
  end
