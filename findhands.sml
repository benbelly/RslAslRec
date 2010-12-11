(*
 *namespace frame_types {
 *    enum {
 *        original = 0,
 *        gray = 1,
 *        skin = 2,
 *        sd = 3,
 *        boundary = 4,
 *        histogram = 5,
 *        special = 6
 *    };
 *}
 *)


val init = _import "InitSarkur" : char vector * int -> unit;
val findHands = _import "findHands" : unit -> unit;
(* addHandImage takes width, height, hand 1 size (number of points), hand 1 points,
   hand 2 size, hand 2 points *)
val addHandsImage = _import "addHandImage" : int * int * int * int vector * int * int vector -> unit;


fun sarkar () =
  let
    val filename="mov/S003.MOV"
    val dataDir = "/home/ben/Documents/school/USF-ASL-Data-Set-v2"
    val dataRoot = AslIO.rootForDir dataDir;
  in
    init( filename, (size filename) );
    (* findHands(); *)
    map (fn (i1, i2) => addHandsImage(460, 290, length i1, Vector.fromList i1,
				      length i2, Vector.fromList i2))
	(AslIO.imagesForRoot dataRoot);

    print "Done.\n"
  end;

sarkar();
