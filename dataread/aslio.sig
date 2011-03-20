signature ASLIO =
sig
    datatype feature = Face of int * int * int * int
             | Dominant of int list
             | Weak of int list
             | Missing;

        (* Frame number and features (Face, Dominant, Weak) *)
    datatype frame = Frame of int * feature * feature * feature;
        (* Directory, gloss, frames *)
    datatype gloss = Gloss of string * string * frame list;
        (* Directory, sentence, sentence number, instance number, glosses *)
    datatype sentence = Sentence of string * string * int * int * gloss list;
        (* Root dir and list of sentences *)
    datatype root = Root of string * sentence list;

    exception BadData of string;

    (*
     * A sentence will have a real number associated with it such as 2.3
     * 2 is the sentence number
     * 3 is the instance number
     *)
        (* Takes sentence, returns the sentence number *)
    val sentenceNum : sentence -> int
        (* Takes sentence, return the instance number *)
    val sentenceInstance : sentence -> int

        (* Takes a directory, return the root tree *)
    val rootForDir : string -> root

        (* Take a root, returns a list of image tuples (dom, weak) *)
    val imagesForRoot : root -> (int list * int list) list
        (* Take a dirname, return a list of point lists [x0,y0,x1,y1,x2,y2] *)
    val handsForDir : string -> int list list

        (* The directory contents are returned in random order,
         * so they need to be sorted *)
    val getSortedCandidates : string -> (string vector * int vector )

end
