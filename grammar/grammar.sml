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

fun g2g word = Gloss(word)
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
    val ss = predecessors grammar item
  in
    Option.isSome (List.find (fn p => p = pred) ss)
  end

