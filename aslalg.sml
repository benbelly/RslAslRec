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
val setDirs = fn(test, train) => fn(_) => (NONE,
       [(NONE,
         Interp.hcons
         { testFrames_H = Interp.testFrames_S.hcnil,
           trainedWords_H = Interp.trainedWords_S.hcnil,
           maxLevel_H = 0, levelNum_H = 0, levelWord_H = "",
           levelScores_H = Interp.levelScores_S.hcnil,
           levelPrevs_H = Interp.levelPrevs_S.hcnil } ) ] );

(* Initialize the system state - training and loading video *)
val trainAndLoad = fn() => fn(i) =>
  (NONE, [(NONE, { testFrames = Vector.foldr op:: [] (Cvsl.getIds 0) } ) ] )

val getWords = fn() => fn(i) => (NONE, [(NONE, { trainedWords = allWords()})]);

val getMaxLevel = fn(is) =>
let
  val imax = foldl Int.max 0 (List.map (fn(i) =>
                                        let
                                          val {levelNum, testDir, trainDir, testFrames,
                                               trainedWords, maxLevel,
                                               levelWord, levelScores, levelPrevs} = i
                                        in levelNum end)
                                     is)
in
  imax
end

val belowMaxLevel = fn (max) => fn(is) => (NONE, Int.<((getMaxLevel(is)),max))

(*[FirstLevel] update levelNum, levelWord, levelScores, levelPrevs: firstLevel()*)
val firstLevel = fn() => fn(_) =>
let
  val start = indexOf Start
  val num = numFramesC(0)
in
  (NONE, [(NONE, { maxLevel = 0, levelNum = 0, levelWord = "",
                   levelScores = List.tabulate(num, fn(_) => 0.0),
                   levelPrevs = List.tabulate(num, fn(_) => 0) })])
end

val atMax = fn() => fn(i) =>
let
  val { maxLevel = max, levelNum = lvl, testDir, trainDir, testFrames,
  trainedWords, levelWord, levelScores, levelPrevs } = i
in
  (NONE, max = lvl)
end
