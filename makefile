SRC_BASE = $(shell pwd)

###############################
#
# CVSL files
#
###############################
CVSL_DIR = $(SRC_BASE)/cvsl
CVSL_OBJS = $(CVSL_DIR)/cvsl.o $(CVSL_DIR)/plotter.o
CVSL_SMLS = $(CVSL_DIR)/cvsl.mlb $(CVSL_DIR)/cvsl.sig $(CVSL_DIR)/cvsl.sml

###############################
#
# ASL recognition files
#
###############################
ASLREC_DIR = $(SRC_BASE)/aslrec
ASLREC_OBJS = $(addprefix $(ASLREC_DIR)/, aslalg.o \
			  							  frame.o keyframeselect.o \
			  							  keyframedist.o \
			  							  skinmask.o edgedetection.o \
										  histograms.o SkinModel.o \
										  SignSeq.o SignSeqScores.o \
										  Gloss.o \
										  FeatureFrame.o \
										  Databases.o FrameDB.o TrainDB.o \
										  aslalgLevel.o TrainingData.o \
										  differenceImageFunctions.o \
										  writer.o HandPairCollection.o)
ASLREC_SMLS = $(addprefix $(ASLREC_DIR)/, aslalgLevel.sml aslalgUtil.sml )


###############################
#
# Levenshtein Distance
#
###############################
LEVENSHTEIN_DIR = $(SRC_BASE)/levenshtein
LEVENSHTEIN_OBJS = $(addprefix $(LEVENSHTEIN_DIR)/, levenshtein.o)

###############################
#
# ASL Data reading files
#
###############################
DATAREAD_DIR = $(SRC_BASE)/dataread
DATAREAD_SMLS = $(addprefix $(DATAREAD_DIR)/, aslio.sig aslio.sml \
											  aslio.mlb \
											  iohelpers.sml iohelpers.mlb \
											  sorting.sml training.sml )

###############################
#
# Grammar files
#
###############################
GRAMMAR_DIR = $(SRC_BASE)/grammar
GRAMMAR_SMLS = $(GRAMMAR_DIR)/grammar.mlb $(GRAMMAR_DIR)/grammar.sml

###############################
#
# Dictionary files
#
###############################
DICTIONARY_DIR = $(SRC_BASE)/dictionary
DICTIONARY_SMLS = $(addprefix $(DICTIONARY_DIR)/, wordscoretable.mlb wordscoretable.sml \
				  								  binary-map-fn.sml lib-base.sml ord-map-sig.sml \
												  lib-base-sig.sml ord-key-sig.sml )

###############################
#
# RSL files
#
###############################
EXEC = levelbuilding
RSL = $(EXEC).rsl
MLB = $(EXEC).mlb
RSLS = $(RSL) $(EXEC)-external.sml lb-report.sml
RSL_GENED = $(EXEC) $(EXEC).i $(EXEC)-types.sml $(EXEC).data $(EXEC)-labels.lrsl $(EXEC).tab $(EXEC).dot $(EXEC).mlb $(EXEC).sml

HANDS = handdetection
HRSL = $(HANDS).rsl
HMLB = $(HANDS).mlb
HRSLS = $(HRSL) hd-interp.sml hd-decision.sml
HAND_GENED = $(HANDS) $(HANDS).i $(HANDS)-types.sml $(HANDS).data $(HANDS)-labels.lrsl $(HANDS).tab $(HANDS).dot $(HANDS).mlb $(HANDS).sml

COUNT = countKeyframes
CRSL = $(COUNT).rsl
CMLB = $(COUNT).mlb
CRSLS = $(CRSL) $(COUNT)funs.sml $(COUNT)help.sml
COUNT_GENED = $(COUNT) $(COUNT).i $(COUNT)-types.sml $(COUNT).data $(COUNT)-labels.lrsl $(COUNT).tab $(COUNT).dot $(COUNT).mlb $(COUNT).sml

SCORE = storeScores
SRSL = $(SCORE).rsl

###
# Compiling
###
OBJS = $(CVSL_OBJS) $(ASLREC_OBJS) $(LEVENSHTEIN_OBJS)

SMLS = $(ASLREC_SMLS) \
	   $(CVSL_SMLS) \
	   $(DATAREAD_SMLS) \
	   $(GRAMMAR_SMLS) \
	   $(DICTIONARY_SMLS) \
	   aslalg.sml aslalg.mlb 

OPENCV_PATH=/home/bholm/raid/OpenCV

##
## C++ variables
##
CC=g++
#DEBUG=-g
OPT=-O3
INCLUDE=-I. -I$(OPENCV_PATH)/include -I$(OPENCV_PATH)/include/opencv \
		-I/usr/lib/mlton/include
CFLAGS=-Werror -Wall -Wextra -c $(INCLUDE) $(DEBUG) $(OPT)

##
## MLTon variables
##
ML = mlton
ML_PATHS = -mlb-path-var "CVSL_DIR $(CVSL_DIR)" \
		   -mlb-path-var "ASLREC_DIR $(ASLREC_DIR)" \
		   -mlb-path-var "DATAREAD_DIR $(DATAREAD_DIR)" \
		   -mlb-path-var "GRAMMAR_DIR $(GRAMMAR_DIR)" \
		   -mlb-path-var "DICTIONARY_DIR $(DICTIONARY_DIR)"
ML_LIBS = -link-opt -lstdc++ \
		  -link-opt "-L$(OPENCV_PATH)/lib" \
		  -link-opt '-lopencv_core -lopencv_highgui -lopencv_ml' \
		  -link-opt '-lopencv_imgproc' \
		  -link-opt '-lplplotcxxd -lplplotd' \
		  -link-opt '-lltdl -ldl -lm -lcsirocsa' \
		  -link-opt '-lcsironn -lqhull -lqsastime -lfreetype' \
		  -link-opt '-lboost_filesystem'
ML_FFI = -default-ann 'allowFFI true'
ML_DEBUG = -const 'Exn.keepHistory true' #-debug true -debug-runtime true
#ML_PERF = -profile time
MLTON_FLAGS = $(ML_PATHS) $(ML_LIBS) $(ML_FFI) $(ML_DEBUG) $(ML_PERF)

##
## RSL variables
##
RSLC = rslc # rslc needs to be in the PATH - see rsl NOTES file

##
## Targets
##
all: $(OBJS) $(EXEC)
		@echo "\nBuild is complete."

%.o: %.cpp
		@echo "\nCompiling $<:"
		$(CC) $(CFLAGS) $< -o $@
		@echo "      built $@."

$(EXEC): $(OBJS) $(SMLS) $(RSLS)
		@echo "\nMaking executable:"
		$(RSLC) $(RSL) $(MLTON_FLAGS) $(OBJS)
		@echo "    executable $@ built."

$(SCORE): $(OBJS) $(SMLS) $(SRSL)
		@echo "\nMaking executable:"
		$(RSLC) $(SRSL) $(MLTON_FLAGS) $(OBJS)
		@echo "    executable $@ built."

$(HANDS): $(OBJS) $(SMLS) $(HRSLS)
		@echo "\nMaking executable:"
		$(RSLC) $(HRSL) $(MLTON_FLAGS) $(OBJS)
		@echo "    executable $@ build."

$(COUNT): $(OBJS) $(SMLS) $(HRSLS)
		@echo "\nMaking executable:"
		$(RSLC) $(CRSL) $(MLTON_FLAGS) $(OBJS)
		@echo "    executable $@ build."


clean: out
		@echo "\nCleaning up..."
		-rm -f *.o *.bak $(CVSL_DIR)/*.o \
			   $(ASLREC_DIR)/*.o $(RSL_GENED) \
			   $(LEVENSHTEIN_DIR)/*.o \
			   $(HAND_GENED)

		@echo "    finished."

out:
		@echo "\nRemoving frames..."
		-rm -f cvsl_out/*
		@echo "    finished."
