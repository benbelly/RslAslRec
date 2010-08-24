EXECUTABLE=findhands
OBJS = findhands.o frame.o keyframeselect.o keyframedist.o skinmask.o \
	   FrameDB.o edgedetection.o logging.o

CC=g++
DEBUG=-g -DTRACE
INCLUDE=-I/home/ben/opencv/include -I/home/ben/opencv/include/opencv #-I/usr/include/opencv 
LIBS=-lcxcore -lcv -lhighgui -lcvaux -lml -L/home/ben/opencv/lib 
CFLAGS=-Wall -c $(DEBUG) $(INCLUDE)
LFLAGS=-Wall $(DEBUG) $(LIBS)

#test: testdiff.cpp
#		$(CC) $(INCLUDE) $(LFLAGS) testdiff.cpp -o testdiff

all: $(OBJS) $(EXECUTABLE)
		@echo "\nBuild is complete."

%.o: %.cpp
		@echo "\nCompiling $<:"
		$(CC) $(CFLAGS) $<
		@echo "      built $@."

$(EXECUTABLE): $(OBJS)
		@echo "\nMaking executable:"
		$(CC) $(OBJS) $(LFLAGS) -o $@
		@echo "    executable $@ built."

clean:
		@echo "\nCleaning up..."
		-rm -f $(EXECUTABLE) *.o *.bak
		@echo "    finished."

cframes:
		@echo "\nRemoving frames..."
		-rm -f output/*
		@echo "    finished."
