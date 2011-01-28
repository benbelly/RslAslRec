
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

(*
 *interp:
 *    testDir: string
 *    trainDir: string
 *    testFrames: int list
 *)
(* functions must return annotations and a set of annotated interpretations *)

(* Add the train and test directory to the interpretation.
 * Called with 'munge', so the type is a little different
 *)
val setDirs = fn( test, train ) => fn(_) => (NONE,
       [(NONE,
         Interp.hcons
         { testDir_H = test, trainDir_H = train,
           testFrames_H = Interp.testFrames_S.hcnil } ) ] );

(* Initialize the system state - training and loading video *)
val trainAndLoad = fn() => fn( i ) =>
let
  val { testDir = test, trainDir = train,
        testFrames = frames } = i
in
  aslalg train test;
  (NONE, [(NONE, { testFrames = Vector.foldr op:: [] (Cvsl.getIds 0) } ) ] )
end

datatype sentenceItem = Start
                      | End
                      | Me
                      | Gloss of string;
