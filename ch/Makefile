CC      = g++
CFLAGS  = -g -fmessage-length=0 -c -Wall -Wextra -pedantic -Wredundant-decls -Wdisabled-optimization -Wctor-dtor-privacy -Wnon-virtual-dtor -Woverloaded-virtual -Wsign-promo -Wold-style-cast -Werror=return-type -DLINUX -std=c++11 -Ofast
SOURCES = main.cpp 
#
SOURCESMAIN = $(SOURCES)
OBJECTSMAIN = $(SOURCESMAIN:.cpp=.o)
#

all: algos

algos: $(OBJECTSMAIN)
	$(CC) $(OBJECTSMAIN) -o ./run.exec
	
.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ 

.cc.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o
	rm -rf run.exec
