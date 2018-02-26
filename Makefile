CC      = g++
BOOST 	= /opt/local/include/
CFLAGS  = -g -fmessage-length=0 -w -c -Wall -D_LINUX -std=c++11 -Ofast -I$(BOOST)
LDFLAGS =
SOURCES = ch/SubgraphCH.cpp graph/graph.cpp graph/partition.cpp pardisp/preprocessing.cpp pardisp/incomponent.cpp pardisp/crosscomponent.cpp pardisp/pardisp.cpp stats/benchmark.cpp main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: algos

algos: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o run.exec $(LDADD) $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@    $(LIBS)

.cc.o:
	$(CC) $(CFLAGS) $< -o $@  $(LIBS)

clean:
	rm -rf *.o
	rm -rf ./preprocessing/*.o
	rm -rf ./ch/*.o
	rm -rf ./stats/*.o
	rm -rf ./graph/*.o
	rm -rf ./pardisp/*.o
	rm -rf run.exec
