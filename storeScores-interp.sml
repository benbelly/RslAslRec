
fun signFromDir dir =
  let
    val lastDir = List.last (String.tokens (fn c => c = #"/") dir)
    val parts = String.tokens (fn c => c = #"_") lastDir
  in
    List.nth(parts, 1)
  end

fun truthInterval is =
  let
    val (begins, ends) = ListPair.unzip (map (fn {interval = i} => i) is)
    (* Add one b/c it's subtracted when we level 0 *)
    val minStart = (foldl Int.min (valOf Int.maxInt) begins) + 1
    val maxEnd = foldl Int.max 0 ends
    val isFullSign = fn (b,e) =>
      let 
        (*
         *val _ = print ("Comparing " ^ (Int.toString b) ^ " to minimum " ^
         *               (Int.toString minStart) ^ "\n\tand " ^ (Int.toString e) ^
         *               " to maximum " ^ (Int.toString maxEnd) ^ "\n")
         *)
        val result = b=minStart andalso e=maxEnd
        (*
         *val _ = print ("\tResult = " ^ (Bool.toString result) ^ "\n")
         *)
      in
        result
      end
  in
    (NONE, (fn {interval} => (NONE, isFullSign interval)))
  end

fun writeInterp (itemMap, testDir) = fn i =>
  let
    val testWord = signFromDir testDir
    val {word, interval = (b,e), score} = i
    val modelWord = sItemToString (itemOf itemMap word)
  in
    (testWord ^ " " ^ modelWord ^ " " ^
     (Int.toString b) ^ " " ^ (Int.toString e) ^ " " ^
     (Real.toString score) ^ "\n")
  end
