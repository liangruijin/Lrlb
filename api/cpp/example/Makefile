CXX = g++
CFLAGS = -std=c++11 -g -O2 -Wall -Wno-deprecated

LRLB_REACTOR=../../../lrlb_reactor
LRLB_REACTOR_H =$(LRLB_REACTOR)/include
LRLB_REACTOR_LIB=$(LRLB_REACTOR)/lib  -llreactor 

all:
	$(CXX) $(CFLAGS) -o example example.cpp -I$(LRLB_REACTOR_H) -I../lrlb_api ../lib/liblrlbclient.a -lprotobuf -lpthread

.PHONY: clean

clean:
	-rm ./example