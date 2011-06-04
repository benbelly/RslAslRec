
datatype sentenceItem = Start
                      | End
                      | ME
                      | Gloss of string;

fun sItemToString Start = "**START**"
  | sItemToString End = "**END**"
  | sItemToString ME = "**ME**"
  | sItemToString (Gloss word) = word

fun stringTosItem "**START**" = Start
  | stringTosItem "**END**" = End
  | stringTosItem "**ME**" = ME
  | stringTosItem word = Gloss word

type entry = sentenceItem * (sentenceItem * sentenceItem) list;

fun pairWithNextTwo is =
  let
    val rec prs = fn (acc, []) => List.rev acc
                   | (acc, _::[]) => List.rev acc
                   | (acc, a::b::[]) => List.rev ((a,b,ME)::acc)
                   | (acc, a::(rst as b::c::_)) => prs ((a,b,c)::acc, rst)
  in
    prs ([], is)
  end

fun pairWithNext is =
  let
    val rec prs = fn (acc, []) => List.rev acc
                   | (acc, _::[]) => List.rev acc
                   | (acc, a::(rst as b::_)) => prs ((a,b)::acc, rst)
  in
    prs ([], is)
  end

exception BadMatch of string
fun entryInsert (((item : sentenceItem),succ1,succ2),(entries : entry list)) =
  let
    val iMatch = fn (i,_) => item = i
    val (matches, nots) = List.partition iMatch entries
    val toE = fn [] => (item, [(succ1,succ2)])
               | (_, ss)::[] => (item,
                                 if (List.exists (fn (s1,s2) => s1 = succ1 andalso s2 = succ2) ss)
                                 then ss
                                 else (succ1,succ2)::ss)
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
    val getVal = fn NONE => let val _ = print ("No match for " ^ (sItemToString item) ^ "\n") in (item, []) end
                  | SOME pair => pair
    val (_, ss) = getVal (List.find (fn (i, _) => i = item) grammar)
  in
    ss
  end

fun root2grammar root =
  let
    val ss = List.map List.rev (r2items root)
    val pairs = List.foldl op@ [] (List.map pairWithNextTwo ss)
    val entries = []
  in
    List.foldl entryInsert entries pairs
  end

fun validPredecessor1 grammar item pred =
  let
    val ss = predecessors grammar item
    val valid = Option.isSome (List.find (fn (p, _) => p = pred) ss)
  in
    valid
  end

fun validPredecessor grammar item pred1 pred2 =
  let 
    val ss = predecessors grammar item
    val valid = Option.isSome (List.find (fn (p1,p2) => p1 = pred1 andalso p2 = pred2) ss)
  in
    valid
  end

fun validSequence grammar seq =
  let
    (*val _ = print ( "Checking: " ^ (String.concatWith " " (List.map sItemToString seq)) ^ "\n")*)
    val noMEs = List.filter (fn ME => false | _ => true) seq
    (* ME can lead / follow anything, so just leave it out *)
    val pairs = pairWithNext noMEs
    val twopairs = pairWithNextTwo noMEs
  in
    (List.all (fn (item, pred) => validPredecessor1 grammar item pred) pairs)
    andalso
    (List.all (fn (item, pred1, pred2) => validPredecessor grammar item pred1 pred2) twopairs)
  end
