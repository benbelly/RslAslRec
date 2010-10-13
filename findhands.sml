(*
 *namespace frame_types {
 *    enum {
 *        original = 0,
 *        gray = 1,
 *        skin = 2,
 *        sd = 3,
 *        boundary = 4,
 *        histogram = 5
 *    };
 *}
 *)


val init = _import "InitSarkur" : char vector * int -> unit;
val findHands = _import "findHands" : unit -> unit;

fun sarkar () =
  let (* Sarkur C functions *)
    val filename="mov/S003.MOV"
  in
    init( filename, (size filename) );
    findHands();
    Cvsl.saveAllImages "cvsl_out/f" "png" 4;
    (*Cvsl.showAllHistograms();*)
    (*saveVideo "cvsl_out/sds.mpeg" 3;*)

    print "Done.\n"
  end;

sarkar();
