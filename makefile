EXECUTABLE=findhands
OBJS = cvsl.o \
	   findhands.o frame.o keyframeselect.o keyframedist.o skinmask.o \
	   FrameDB.o edgedetection.o logging.o histograms.o plotter.o \
	   eigenthingummies.o #SignDB.o
MLB = findhands.mlb
SMLS = findhands.mlb cvsl.sml cvsl.mlb

#OPENCV_PATH=/home/ben/opencv
OPENCV_PATH=/usr/include/opencv
PLPLOT_PATH=/home/ben/plplot

##
## C++ variables
CC=g++
DEBUG=-g -DTRACE
INCLUDE=-I$(OPENCV_PATH)/include -I$(OPENCV_PATH)/include/opencv \
		-I$(PLPLOT_PATH)/include \
		-I/usr/lib/mlton/include
CFLAGS=-Wall -c $(DEBUG) $(INCLUDE)

##
## MLTon variables
ML = mlton
#ML_LIBS = -link-opt -L$(OPENCV_PATH)/lib \
		  #-link-opt -L$(PLPLOT_PATH)/lib 
ML_LIBS = -link-opt -lstdc++ \
		  -link-opt -lcxcore -link-opt -lcv -link-opt -lhighgui \
		  -link-opt -lcvaux -link-opt -lml \
		  -link-opt -lavutil \
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

$(EXECUTABLE): $(OBJS) $(SMLS) $(MLB)
		@echo "\nMaking executable:"
		$(ML) $(ML_LIBS) $(ML_FFI) $(MLB) $(OBJS)
		@echo "    executable $@ built."

clean:
		@echo "\nCleaning up..."
		-rm -f $(EXECUTABLE) *.o *.bak
		@echo "    finished."

output:
		@echo "\nRemoving frames..."
		-rm -f cvsl_out/*
		@echo "    finished."
