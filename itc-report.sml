(**
    Copyright 2011 Ben Holm

    This file is part of RslAslRec.

    RslAslRec is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RslAslRec is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RslAslRec.  If not, see <http://www.gnu.org/licenses/>.
**)

fun dirname file =
  let
    val elements = String.tokens (fn c => c = #"/") file
    val lastDropped = List.take(elements, (List.length elements)-1)
    val dir = "/" ^ (String.concatWith "/" lastDropped)
  in
    dir
  end

fun lineToTuple line =
  let
    (*val _ = print (line ^ "\n")*)
    val wrdNint = String.tokens Char.isSpace line
    val wrd = hd wrdNint
    val intStr = hd (tl wrdNint)
    val splitNums = String.tokens (fn c => c = #"-") intStr
    val nums = List.map valOf (List.map Int.fromString splitNums)
    val b = List.nth(nums, 0)
    val e = List.nth(nums, 1)
  in
    (wrd, b, e)
  end

exception BadInterpretation of string

fun alphaForFile file =
  let
    (*val _ = print file*)
    val fname = List.last (String.tokens (fn c => c = #"/") file)
    val alpha = valOf (Real.fromString (List.nth(
                                    String.tokens (fn c => c = #"-") fname, 1)))
  in
    alpha
  end

fun nofile {file} = (NONE, file = "NONE")

fun alphaStr a = Real.toString a
fun strAlpha astr = valOf (Real.fromString astr)
fun levelStr l = Int.toString l
fun strLevel ls = valOf (Int.fromString ls)
fun wordStr w = w
fun strWord ws = ws
fun scoreStr s = Real.toString s
fun strScore ss = valOf (Real.fromString ss)
fun intervalStr (b,e) = "(" ^ (Int.toString b) ^ ", " ^ (Int.toString e) ^ ")"
fun strInterval is =
  let
    val strs = String.tokens Char.isSpace is
    val sploded = List.map String.explode strs
    val clean = List.map (List.filter (fn c => Char.isDigit c)) sploded
    val imploded = List.map String.implode clean
    val numList = List.map valOf (List.map Int.fromString imploded)
  in
    (* There better be a first and second! *)
    (List.nth(numList, 0), List.nth(numList, 1))
  end

fun truthStr truth =
  let
    val _ = "[Lipread|(54,68):Can|(72,80)]"
    val pStr = fn (wrd, b, e) => (wordStr wrd) ^ "|" ^ (intervalStr (b,e))
    val pStrs = List.map pStr truth
    val listContents = String.concatWith ":" pStrs
  in
    "[" ^ listContents ^ "]"
  end

fun strTruth trth =
  let
    val lstStr = String.substring(trth, 1, (String.size trth) - 2)
    val strTuples = String.tokens (fn c => c = #":") lstStr
    val wrdIntLst = List.map (String.tokens (fn c => c = #"|")) strTuples
    val wrdTupleLst = List.map (fn wrd::tple::[] => (wrd, tple)
                                 | _ => raise BadInterpretation "invalid prevs format") wrdIntLst
    val truthList = List.map (fn (wrd, interval) =>
                                    let val (b,e) = strInterval interval in (wrd, b, e) end)
                             wrdTupleLst
  in
    truthList
  end


fun prevsStr (ps, pscr) =
  let
    val _ = "[Lipread|(54, 68):Can|(72, 80)]@3.1415927"
    val listContents = truthStr ps
  in
    listContents ^ "@" ^ (scoreStr pscr)
  end

fun strPrev pstr =
  let
    val _ = "[Lipread|(54, 68):Can|(72, 80)]@3.1415927"
    val lstNscr = String.tokens (fn c => c = #"@") pstr
    val lstStrBrackets = List.nth(lstNscr,0)
    val ps = strTruth lstStrBrackets
    val pscr = strScore (List.nth(lstNscr,1))
  in
    (ps, pscr)
  end

fun truthForFile infile =
  let
    val dir = dirname infile
    val file = dir ^ "/intervals"
    val full = "Full"
    val instream = TextIO.openIn file
    val readLine = fn() => TextIO.inputLine instream 
    val rec truths = fn(b,e,acc,NONE)=> ((b,e),rev acc)
                      |(_,_,acc,SOME line)=>
                          if (String.isPrefix full line)
                            then let
                                   val (_, tb, te) = lineToTuple line
                                 in
                                   truths (tb, te, acc, NONE)
                                 end
                            else truths (0,0,(lineToTuple line)::acc, readLine())
    val truth = truths(0,0,[],(readLine()))
    val _ = TextIO.closeIn instream
  in
    truth
  end

fun loadTruth file = fn _ =>
  let
    val (range, truth) = truthForFile file
    val alpha = alphaForFile file
  in
    (NONE, [(NONE, { alpha = alpha, file = file, range = range, truth = truth })])
  end

fun interpToString i =
  let
    val { file = _, alpha, truth, range, level , word, score, interval, prevs } = i
  in
    (alphaStr alpha) ^ "\n" ^
    (truthStr truth) ^ "\n" ^
    (intervalStr range) ^ "\n" ^
    (levelStr level) ^ "\n" ^
    (wordStr word) ^
    (scoreStr score) ^ "\n" ^
    (intervalStr interval) ^ "\n" ^
    (prevsStr prevs)
  end

fun streamToInterp strm =
  let
    val read = fn () => let
                          val line = valOf (TextIO.inputLine strm)
                          (*val _ = print (line)*)
                        in
                          line
                        end
    (*
     *val alpha = strAlpha (read())
     *val truth = strTruth (read())
     *)
    val level = strLevel (read())
    val word = strWord (read())
    val score = strScore (read())
    val interval = strInterval (read())
    val prevs = strPrev (read())
  in
    SOME (NONE, { file = "NONE", alpha = 0.0, truth = [], range = (0,0), level = level, word = word,
                  score = score, interval = interval, prevs = prevs } )
  end

