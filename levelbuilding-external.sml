
val interpToString = fn _ =>
let
(*
 *  val { testFrames, levelNum,
 *        levelWord, levelScores,
 *        levelPrevs } = i
 *  val spaceCat = fn (a, b) => a ^ " " ^ b
 *  val commaCat = fn (a, b) => a ^ ", " ^ b
 *  [>val prev2Str = fn (w, s) => (List.foldr spaceCat "" w) ^ "@" ^ (Real.toString s)<]
 *  *)
 in
   "Well now, I just don't know yet\n"
 (*      [> Frame ids <]
 *  "FrameIds = [" ^ (Vector.foldr commaCat "" (Vector.map Int.toString testFrames)) ^ "]\n" ^
 *      [> Level <]
 *  "level " ^ (Int.toString levelNum) ^ "\n" ^
 *  "levelWord = " ^ Int.toString(levelWord) ^ "\n" ^
 *      [> Scores <]
 *  "[" ^ (Vector.foldr commaCat "" (Vector.map Real.toString levelScores)) ^ "]\n"
 *      [> Sentence <]
 *  [>^ "Sentence so far: " ^ (List.foldr spaceCat "" (List.rev soFar)) ^ "\n"<]
 *      [> Previous score <]
 *  [>^ "[" ^ (List.foldr commaCat "" (List.map prev2Str levelPrevs)) ^ "]\n"<]
 *)
end
