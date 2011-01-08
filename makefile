EXECUTABLE=aslalg
OBJS = cvsl.o \
	   aslalg.o frame.o keyframeselect.o keyframedist.o skinmask.o \
	   FrameDB.o edgedetection.o histograms.o plotter.o \
	   eigens.o SignDB.o \
	   TrainDB.o SignSeq.o Gloss.o \
	   SkinModel.o FeatureFrame.o \
	   Databases.o aslalgLevel.o \
	   LevelArrays.o SignSeqScores.o \

# LevelBuilding.o

SMLS = aslalg.sml aslalg.mlb \
	   aslalgLevel.sml \
	   cvsl.sig cvsl.sml cvsl.mlb \
	   aslio.sig aslio.sml aslio.mlb \
	   iohelpers.sml iohelpers.mlb \
	   sorting.sml

MLB = aslalg.mlb

#OPENCV_PATH=/home/ben/opencv
OPENCV_PATH=/usr/include/opencv
PLPLOT_PATH=/usr

##
## C++ variables
#CC=g++-4.5
CC=g++
DEBUG=-g
INCLUDE=-I$(OPENCV_PATH)/include -I$(OPENCV_PATH)/include/opencv \
		-I$(PLPLOT_PATH)/include \
		-I/usr/lib/mlton/include
# CFLAGS=-Wall -Wextra -std=c++0x -c $(INCLUDE) $(DEBUG)
CFLAGS=-Wall -Wextra -c $(INCLUDE) $(DEBUG)

##
## MLTon variables
ML = mlton
ML_LIBS = -link-opt -lstdc++ \
		  -link-opt -lcxcore -link-opt -lcv -link-opt -lhighgui \
		  -link-opt -lcvaux -link-opt -lml \
		  -link-opt -lavutil \
		  -link-opt -L$(PLPLOT_PATH)/lib \
		  -link-opt -lplplotcxxd -link-opt -lplplotd \
		  -link-opt -lltdl -link-opt -ldl \
		  -link-opt -lm -link-opt -lcsirocsa \
		  -link-opt -lcsironn -link-opt -lqhull \
		  -link-opt -lqsastime -link-opt -lfreetype
ML_FFI = -default-ann 'allowFFI true'
ML_DEBUG = -const 'Exn.keepHistory true'

all: $(OBJS) $(EXECUTABLE)
		@echo "\nBuild is complete."

%.o: %.cpp
		@echo "\nCompiling $<:"
		$(CC) $(CFLAGS) $<
		@echo "      built $@."

$(EXECUTABLE): $(OBJS) $(SMLS) $(MLB)
		@echo "\nMaking executable:"
		$(ML) $(ML_LIBS) $(ML_FFI) $(ML_DEBUG) $(MLB) $(OBJS)
		@echo "    executable $@ built."

clean: out
		@echo "\nCleaning up..."
		-rm -f $(EXECUTABLE) *.o *.bak
		@echo "    finished."

out:
		@echo "\nRemoving frames..."
		-rm -f cvsl_out/*
		@echo "    finished."
