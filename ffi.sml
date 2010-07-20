
val loadFrames = _import "loadFrames": char vector * int -> int;
val getFrames = _import "getFrames" : int array -> unit;

fun cstring s = Vector.fromList( explode( s ) );

fun tk( _, 0 ) = []
  | tk( v, n ) = Vector.sub(v, n) :: tk( v, n-1 );

val filename="mov/S003.MOV";

val count = loadFrames( cstring( filename ), (size filename) );

val frameIds : Int32.int array = Array.array( count, 0 );
getFrames( frameIds );

print ( "Ids:" ^ ( Int.toString( count ) ) ^ "\n" );

Vector.app print (Vector.map (fn i  => Int.toString(i) ^ ", ") (Array.vector frameIds));
print "\n";

