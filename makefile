EXECUTABLE=findhands
OBJS = cvsl.o \
	   findhands.o frame.o keyframeselect.o keyframedist.o skinmask.o \
	   FrameDB.o edgedetection.o logging.o histograms.o
SMLS = findhands.mlb

##
## C++ variables
CC=g++
DEBUG=-g -DTRACE
INCLUDE=-I/home/ben/opencv/include -I/home/ben/opencv/include/opencv -I/usr/lib/mlton/include #-I/usr/include/opencv 
LIBS=-lcxcore -lcv -lhighgui -lcvaux -lml -L/home/ben/opencv/lib 
CFLAGS=-Wall -c $(DEBUG) $(INCLUDE)
LFLAGS=-Wall $(DEBUG) $(LIBS)

##
## MLTon variables
ML = mlton
ML_LIBS = -link-opt -L/home/ben/opencv/lib \
		  -link-opt -lstdc++ \
		  -link-opt -lcxcore -link-opt -lcv -link-opt -lhighgui \
		  -link-opt -lcvaux -link-opt -lml
ML_FFI = -default-ann 'allowFFI true'

all: $(OBJS) $(EXECUTABLE)
		@echo "\nBuild is complete."

%.o: %.cpp
		@echo "\nCompiling $<:"
		$(CC) $(CFLAGS) $<
		@echo "      built $@."

$(EXECUTABLE): $(OBJS) $(SMLS)
		@echo "\nMaking executable:"
		$(ML) $(ML_LIBS) $(ML_FFI) $(SMLS) $(OBJS)
		@echo "    executable $@ built."

clean:
		@echo "\nCleaning up..."
		-rm -f $(EXECUTABLE) *.o *.bak
		@echo "    finished."

cframes:
		@echo "\nRemoving frames..."
		-rm -f output/*
		@echo "    finished."
