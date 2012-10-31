Implementation of an American Sign Language (ASL) Recognition Algorithm using RSL

This repository contains the code I wrote for my thesis in pursuit of my Masters in Computer Science. It is released under the Lesser Gnu Public License (see COPYING and COPYING.LESSER) except for three specific files:

    levenshtein/levenshtein.*
    datareader/sorting.sml

Those three files were example code I pulled from other sources, and have their own licenses.


The thesis document can be found [here](https://ritdml.rit.edu/handle/1850/14475 "Evaluation of RSL history as a tool for assistance in the development and evaluation of computer vision algorithms"), and the algorithm being implemented is described in:

Barbara Loeding, Ruiduo Yang, Sudeep Sarkar. Handling movement epenthesis and hand segmentation ambiguities in continuous sign language recognition using nested dynamic programming. IEEE Transactions on Pattern Analysis and Machine Intelligence, 32(3), 2010.

The language used to implement and examine the characteristics of the ASL recognition algorithm is [RSL](https://ritdml.rit.edu/bitstream/handle/1850/13886/RZanibbiArticle03-2011.pdf?sequence=1 "RSL"), which is built to ML, then compiled to machine code using the mlton compiler. There are three main types of files here:

* *.rsl - these are the RSL files implementing various parts of the ASL recognition algorithm.
* *.sml - these are Standard ML files containing functions called from RSL
* *.h|cpp - C++ files and function called from Standard ML called from RSL

The programs in this repository depend on the OpenCV libraries and the boost libraries. If there are others, and I think of them, I will note them here.

To build this code you will need to get access to the RSL compiler. If you have any questions, send benbelly an email at Gmail.

