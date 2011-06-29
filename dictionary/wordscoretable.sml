type key = (int * int)
type value = real

fun keyCompare (kl : key, kr: key) =
  let
    val (wordl, endl) = kl
    val (wordr, endr) = kr
  in
    case Int.compare(wordl, wordr) of
         LESS => LESS
       | GREATER => GREATER
       | EQUAL => Int.compare(endl, endr)
  end

structure WordScoreTable = BinaryMapFn( struct type ord_key = key
                                               val compare = keyCompare
                                        end)
