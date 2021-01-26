TARGET= reportClient
CXX=g++
CFLAGS=-std=c++11 -g -O2 -Wall -Wno-deprecated

BASE=../../base
BASE_H=$(BASE)/include

PROTO = $(BASE)/proto
PROTO_H = $(BASE)/proto

LRLB_REACTOR=../../lrlb_reactor
LRLB_REACTOR_H =$(LRLB_REACTOR)/include
LRLB_REACTOR_LIB=$(LRLB_REACTOR)/lib  -llreactor

OTHER_LIB = -lpthread -ldl -lprotobuf
SRC= ./src
INC= -I./include -I$(BASE_H) -I$(LRLB_REACTOR_H) -I$(PROTO_H)

LIB= $(MYSQL_LIB) -L$(LRLB_REACTOR_LIB) $(OTHER_LIB) 


OBJS = reportClient.o
OBJS += $(PROTO)/lrlb.pb.o

$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) -o $(TARGET) $(OBJS) $(INC) $(LIB)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $< $(INC) 

.PHONY: clean

clean:
	-rm -f ./*.o $(TARGET)
