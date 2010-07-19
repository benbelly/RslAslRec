
val getFrameIds = _import "getFrameIds": char array * int -> int vector;

val filename="mov/S003.MOV";

val ids = getFrameIds( Array.fromList( explode filename ), (size filename) );

print ( "Ids:" ^ ( Int.toString( Vector.length ids ) ) ^ "\n" );
Vector.map print (Vector.map Int.toString ids );
