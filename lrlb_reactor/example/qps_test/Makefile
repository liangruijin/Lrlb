CXX=g++
CFLAGS=-g -std=c++11 -O2 -Wall -fPIC -Wno-deprecated

INC=-I../../include
LIB=-L../../lib -llreactor -lpthread -lprotobuf
OBJS = $(addsuffix .o, $(basename $(wildcard *.cc)))
all:
	$(CXX) -o server $(CFLAGS)  server.cpp echoMessage.pb.cc $(INC) $(LIB)
	$(CXX) -o client $(CFLAGS)  client.cpp echoMessage.pb.cc $(INC) $(LIB)
	
clean:
	rm -f *.o server client