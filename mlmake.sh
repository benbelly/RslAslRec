#!/bin/sh
mlton \
      -link-opt -L/home/ben/opencv/lib \
      -link-opt -lstdc++ \
      -link-opt -lcxcore -link-opt -lcv -link-opt -lhighgui -link-opt -lcvaux -link-opt -lml \
      -default-ann 'allowFFI true' \
      ffi.sml \
      findhands.o frame.o edgedetection.o keyframedist.o \
      keyframeselect.o  logging.o  skinmask.o
