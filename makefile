EXECUTABLE=aslalg

CVSL_DIR=cvsl
CVSL_OBJS = $(CVSL_DIR)/cvsl.o $(CVSL_DIR)/plotter.o
CVSL_SMLS = $(CVSL_DIR)/cvsl.mlb $(CVSL_DIR)/cvsl.sig $(CVSL_DIR)/cvsl.sml

OBJS = $(CVSL_OBJS) \
	   aslalg.o frame.o keyframeselect.o keyframedist.o skinmask.o \
	   FrameDB.o edgedetection.o histograms.o \
	   eigens.o SignDB.o \
	   TrainDB.o SignSeq.o Gloss.o \
	   SkinModel.o FeatureFrame.o \
	   Databases.o aslalgLevel.o \
	   LevelArrays.o SignSeqScores.o \

# LevelBuilding.o

SMLS = aslalg.sml aslalg.mlb \
	   aslalgLevel.sml \
	   $(CVSL_SMLS) \
	   aslio.sig aslio.sml aslio.mlb \
	   iohelpers.sml iohelpers.mlb \
	   sorting.sml training.sml

MLB = aslalg.mlb

#OPENCV_PATH=/home/ben/opencv
OPENCV_PATH=/usr/include/opencv
PLPLOT_PATH=/usr

##
## C++ variables
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
ML_PATHS = -mlb-path-var "CVSL_DIR $(CVSL_DIR)"
ML_LIBS = -link-opt -lstdc++ \
		  -link-opt '-lcxcore -lcv -lhighgui -lcvaux -lml -lavutil' \
		  -link-opt '-L$(PLPLOT_PATH)/lib -lplplotcxxd -lplplotd' \
		  -link-opt '-lltdl -ldl -lm -lcsirocsa' \
		  -link-opt '-lcsironn -lqhull -lqsastime -lfreetype'
ML_FFI = -default-ann 'allowFFI true'
ML_DEBUG = -const 'Exn.keepHistory true'

all: $(OBJS) $(EXECUTABLE)
		@echo "\nBuild is complete."

%.o: %.cpp
		@echo "\nCompiling $<:"
		$(CC) $(CFLAGS) $< -o $@
		@echo "      built $@."

$(EXECUTABLE): $(OBJS) $(SMLS) $(MLB)
		@echo "\nMaking executable:"
		$(ML) $(ML_PATHS) $(ML_LIBS) $(ML_FFI) $(ML_DEBUG) $(MLB) $(OBJS)
		@echo "    executable $@ built."

clean: out
		@echo "\nCleaning up..."
		-rm -f $(EXECUTABLE) *.o *.bak
		@echo "    finished."

out:
		@echo "\nRemoving frames..."
		-rm -f cvsl_out/*
		@echo "    finished."
