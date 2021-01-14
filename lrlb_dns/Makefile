TARGET= bin/lars_dns
CXX=g++
CFLAGS=-g -std=c++11 -O2 -Wall -Wno-deprecated

BASE=../base
BASE_H=$(BASE)/include
LRLB_REACTOR=../lrlb_reactor
LRLB_REACTOR_H =$(LRLB_REACTOR)/include
LRLB_REACTOR_LIB=$(LRLB_REACTOR)/lib  -llreactor

MYSQL=$(BASE)/mysql-connector-c
MYSQL_H=$(MYSQL)/include
MYSQL_LIB=$(MYSQL)/lib/libmysqlclient.a


OTHER_LIB = -lpthread -ldl
SRC= ./src
INC= -I./include -I$(BASE_H) -I$(LRLB_REACTOR_H) -I$(MYSQL_H)

LIB= $(MYSQL_LIB) -L$(LRLB_REACTOR_LIB) $(OTHER_LIB)


OBJS = $(addsuffix .o, $(basename $(wildcard $(SRC)/*.cpp)))

$(TARGET): $(OBJS)
	mkdir -p bin
	$(CXX) $(CFLAGS) -o $(TARGET) $(OBJS) $(INC) $(LIB)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $< $(INC) 

.PHONY: clean

clean:
	rm -f src/*.o $(TARGET)
