EXECUTABLE=findhands
OBJS = cvsl.o \
	   findhands.o frame.o keyframeselect.o keyframedist.o skinmask.o \
	   FrameDB.o edgedetection.o logging.o histograms.o
SMLS = findhands.mlb

OPENCV_PATH=/home/ben/opencv

##
## C++ variables
CC=g++
DEBUG=-g -DTRACE
INCLUDE=-I$(OPENCV_PATH)/include -I$(OPENCV_PATH)/include/opencv -I/usr/lib/mlton/include #-I/usr/include/opencv 
CFLAGS=-Wall -c $(DEBUG) $(INCLUDE)
LFLAGS=-Wall $(DEBUG) $(LIBS)

##
## MLTon variables
ML = mlton
ML_LIBS = -link-opt -L$(OPENCV_PATH)/lib \
		  -link-opt -lstdc++ \
		  -link-opt -lcxcore -link-opt -lcv -link-opt -lhighgui \
		  -link-opt -lcvaux -link-opt -lml \
		  -link-opt -lplplotcxxd -link-opt -lplplotd \
		  -link-opt -lltdl -link-opt -ldl \
		  -link-opt -lm -link-opt -lcsirocsa \
		  -link-opt -lcsironn -link-opt -lqhull \
		  -link-opt -lqsastime -link-opt -lfreetype
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

output:
		@echo "\nRemoving frames..."
		-rm -f cvsl_out/*
		@echo "    finished."
