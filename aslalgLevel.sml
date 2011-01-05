
local
val numberOfSequencesC = _import "numberOfSequencesC" : string * int -> int;
val getSequencesC = _import "getSequencesC" : string * int * (int ref) array -> unit;
val distanceC = _import "distanceC" : int ref * int * int -> real;
val getMaxScoreC = _import "getMaxScoreC" : unit -> real;
in

fun sequencesForGloss gloss =
  let
    val seqCount = numberOfSequencesC( gloss, size gloss )
    val seqs = Array.array( seqCount, ref 0 )
    val _ = getSequencesC( gloss, size gloss, seqs )
  in
    Array.vector seqs
  end;

fun distanceForGloss gloss first last: real =
  let
    val m = getMaxScoreC()
    val seqs = sequencesForGloss gloss
  in
    Vector.foldl (fn (s,m) => Real.min(m, distanceC(s, first, last)))
                 (getMaxScoreC()) seqs
  end;
end
