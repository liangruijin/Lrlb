CXX=g++
CFLAGS=-g -std=c++11 -O2 -Wall -fPIC -Wno-deprecated 

INC=-I../../include
LIB=-L../../lib -llreactor -lpthread
OBJS = $(addsuffix .o, $(basename $(wildcard *.cc)))

all:
	$(CXX) -o lrlb_reactor $(CFLAGS)  lrlb_reactor.cpp $(INC) $(LIB)

clean:
	rm -f *.o lrlb_reactor

