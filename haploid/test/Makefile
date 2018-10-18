TARGET=sim

CXX=g++

CXXFLAGS=-std=c++14 -g -ggdb -Wall -O3 -lhaploid
INCLUDE=-I../include -I../../common/include -I/usr/local/include 
LIBRARY=-L../lib/

$(TARGET):sim.cc
			 $(CXX) $^ -o $@ $(CXXFLAGS) $(INCLUDE) $(LIBRARY)
clean:
	  ${RM} $(OBJS) $(TARGET) 
