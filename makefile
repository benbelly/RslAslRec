EXECUTABLE=aslalg

###############################
#
# CVSL files
#
###############################
CVSL_DIR = cvsl
CVSL_OBJS = $(CVSL_DIR)/cvsl.o $(CVSL_DIR)/plotter.o
CVSL_SMLS = $(CVSL_DIR)/cvsl.mlb $(CVSL_DIR)/cvsl.sig $(CVSL_DIR)/cvsl.sml

###############################
#
# ASL recognition files
#
###############################
ASLREC_DIR = aslrec
ASLREC_OBJS = $(addprefix $(ASLREC_DIR)/, aslalg.o \
			  							  frame.o keyframeselect.o \
			  							  keyframedist.o \
			  							  skinmask.o edgedetection.o \
										  eigens.o \
										  histograms.o SkinModel.o \
										  SignSeq.o SignSeqScores.o \
										  Gloss.o \
										  FeatureFrame.o \
										  Databases.o FrameDB.o TrainDB.o \
										  aslalgLevel.o )
ASLREC_SMLS = $(addprefix $(ASLREC_DIR)/, aslalgLevel.sml )


###############################
#
# ASL Data reading files
#
###############################
DATAREAD_DIR = dataread
DATAREAD_SMLS = $(addprefix $(DATAREAD_DIR)/, aslio.sig aslio.sml \
											  aslio.mlb \
											  iohelpers.sml iohelpers.mlb \
											  sorting.sml training.sml )

###############################
#
# RSL files
#
###############################
EXEC = levelbuilding
RSL = $(EXEC).rsl
#MLB = $(EXEC).mlb
MLB = aslalg.mlb
RSL_GENED = $(EXEC)-types.sml $(EXEC)-labels.rsl $(EXEC).mlb $(EXEC).sml

###
# Compiling
###
OBJS = $(CVSL_OBJS) $(ASLREC_OBJS)

SMLS = $(ASLREC_SMLS) \
	   $(CVSL_SMLS) \
	   $(DATAREAD_SMLS) \
	   aslalg.sml aslalg.mlb 

OPENCV_PATH=/home/bholm/OpenCV

##
## C++ variables
##
CC=g++
DEBUG=-g
INCLUDE=-I. -I$(OPENCV_PATH)/include -I$(OPENCV_PATH)/include/opencv \
		-I/usr/lib/mlton/include
CFLAGS=-Wall -Wextra -c $(INCLUDE) $(DEBUG)

##
## MLTon variables
##
ML = mlton
ML_PATHS = -mlb-path-var "CVSL_DIR $(CVSL_DIR)" \
		   -mlb-path-var "ASLREC_DIR $(ASLREC_DIR)" \
		   -mlb-path-var "DATAREAD_DIR $(DATAREAD_DIR)"
ML_LIBS = -link-opt -lstdc++ \
		  -link-opt '-L/home/bholm/OpenCV/lib' \
		  -link-opt '-lcxcore -lcv -lhighgui -lcvaux -lml -lavutil' \
		  -link-opt '-lplplotcxxd -lplplotd' \
		  -link-opt '-lltdl -ldl -lm -lcsirocsa' \
		  -link-opt '-lcsironn -lqhull -lqsastime -lfreetype'
ML_FFI = -default-ann 'allowFFI true'
ML_DEBUG = -const 'Exn.keepHistory true'
MLTON_FLAGS = $(ML_PATHS) $(ML_LIBS) $(ML_FFI) $(ML_DEBUG)

##
## RSL variables
##
RSLC = rslc # rslc needs to be in the PATH - see rsl NOTES file

##
## Targets
##
all: $(OBJS) $(EXECUTABLE)
		@echo "\nBuild is complete."

%.o: %.cpp
		@echo "\nCompiling $<:"
		$(CC) $(CFLAGS) $< -o $@
		@echo "      built $@."

$(EXECUTABLE): $(OBJS) $(SMLS)
		@echo "\nMaking executable:"
		#$(RSLC) $(RSL) 
		#$(ML) -mlb-path-var "RSL2_DIR /home/bholm/rsl" $(MLTON_FLAGS) $(MLB) $(OBJS)
		$(ML) $(MLTON_FLAGS) $(MLB) $(OBJS)
		@echo "    executable $@ built."

clean: out
		@echo "\nCleaning up..."
		-rm -f $(EXECUTABLE) *.o *.bak $(CVSL_DIR)/*.o $(ASLREC_DIR)/*.o $(RSL_GENED)
		@echo "    finished."

out:
		@echo "\nRemoving frames..."
		-rm -f cvsl_out/*
		@echo "    finished."
