TARGET=sim

CXX=g++
CXXFLAGS=-std=c++14 -g -O3 -ggdb -Wall
INCLUDE=-I./include -I/usr/local/include/nlohmann/
OBJS=obj/node.o obj/geneset.o obj/sim.o

$(TARGET):$(OBJS)
		    $(CXX) -o $@ $(CXXFLAGS) $(INCLUDE) $^

obj/node.o:src/node.cc include/node.hh
			     $(CXX) -o $@ $(CXXFLAGS) $(INCLUDE) $< -c
obj/geneset.o:src/geneset.cc include/geneset.hh
			     $(CXX) -o $@ $(CXXFLAGS) $(INCLUDE) $< -c
obj/sim.o:src/sim.cc 
			  $(CXX) -o $@ $(CXXFLAGS) $(INCLUDE) $< -c

clean:
	  ${RM} $(OBJS) $(TARGET)
