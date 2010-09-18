(*
 *namespace frame_types {
 *    enum {
 *        original = 0,
 *        gray = 1,
 *        skin = 2,
 *        sd = 3,
 *        boundary = 4
 *    };
 *}
 *)


val init = _import "InitSarkur" : char vector * int -> unit;
val findHands = _import "findHands" : unit -> unit;

fun sarkar () =
let (* Sarkur C functions *)
  val filename="mov/08.avi"
in
  init( filename, (size filename) );
  findHands();
  (*Cvsl.displayVideo 0;*)
  Cvsl.displayVideo 4;
  Cvsl.saveImage "cvsl_out/f10.png" 0 10;
  Cvsl.saveAllImages "cvsl_out/gray" "png" 1;
  (*saveVideo "cvsl_out/sds.mpeg" 3;*)

  print "Done.\n"
end;

sarkar();
