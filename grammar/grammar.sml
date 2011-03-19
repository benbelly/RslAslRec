fun pairWithNext is =
  let
    val rec prs = fn (acc, []) => List.rev acc
                   | (acc, _::[]) => List.rev acc
                   | (acc, a::(rst as b::_)) => prs ((a,b)::acc, rst)
  in
    prs ([], is)
  end

datatype sentenceItem = Start
                      | End
                      | ME
                      | Gloss of string;

fun sItemToString Start = "**START**"
  | sItemToString End = "**END**"
  | sItemToString ME = "**ME**"
  | sItemToString (Gloss word) = word

type entry = sentenceItem * sentenceItem list;

exception BadMatch of string
fun entryInsert (((item : sentenceItem),succ),(entries : entry list)) =
  let
    val iMatch = fn (i,_) => item = i
    val (matches, nots) = List.partition iMatch entries
    val toE = fn [] => (item, [succ])
               | (_, ss)::[] => (item, if (List.exists (fn s => s = succ) ss)
                                       then ss
                                       else succ::ss)
               | _ => raise BadMatch "too many matches"
  in
    (toE matches)::nots
  end

fun s2items (AslIO.Sentence(_, s, _, _, _)) =
  let
    val words = String.tokens Char.isSpace s
  in
    Start::((map (fn w => (Gloss w)) words)@[End])
  end

fun r2items (AslIO.Root(_,ss)) = map s2items ss

fun predecessors grammar item =
  let
    val (_, ss) = valOf (List.find (fn (i, _) => i = item) grammar)
  in
    ss
  end

fun root2grammar root =
  let
    val ss = List.map List.rev (r2items root)
    val pairs = List.foldl op@ [] (List.map pairWithNext ss)
    val entries = []
  in
    List.foldl entryInsert entries pairs
  end

fun validPredecessor grammar item pred =
  let 
    val iword = sItemToString item
    val pword = sItemToString pred
    val _ = print ("Checking " ^ pword ^ " before " ^ iword ^ "\n")
    val ss = predecessors grammar item
    val valid = Option.isSome (List.find (fn p => p = pred) ss)
    val is = if valid then " is " else " is not "
    val _ = print (pword ^ is ^ "a valid predecessor of " ^ iword ^ "\n")
  in
    valid
  end

fun validSequence grammar seq depth =
  let
    val toTake = Int.min(depth, List.length seq)
    val pairs = pairWithNext (List.take(seq, toTake))
    val _ = print ("Validating sequence of size " ^ (Int.toString(List.length seq)) ^
                   " at depth " ^ (Int.toString(depth)) ^ "\n")
    val _ = print ("sizeof pairs = " ^ (Int.toString(List.length pairs)) ^ "\n")
  in
    List.all (fn (item, pred) => validPredecessor grammar item pred) pairs
  end
