TARGET=sim

CXX=g++
CXXFLAGS=-std=c++11 -larmadillo -g -ggdb -Wall -O3
INCLUDE=-I./include -I/usr/local/include/nlohmann/
OBJS=obj/node.o obj/geneset.o obj/sim.o

$(TARGET):$(OBJS)
		    $(CXX) $^ -o $@ $(CXXFLAGS) $(INCLUDE) 

obj/node.o:src/node.cc include/node.hh
			     $(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE) 
obj/geneset.o:src/geneset.cc include/geneset.hh
			     $(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE) 
obj/sim.o:src/sim.cc 
			     $(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE) 
clean:
	  ${RM} $(OBJS) $(TARGET)
