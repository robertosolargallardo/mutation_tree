TARGET=sim

CXX=g++
CXXFLAGS=-std=c++14 -g -ggdb -Wall -O3
INCLUDE=-I./include -I/usr/local/include/nlohmann/
OBJS=obj/node.o obj/sim.o obj/pop.o obj/gene.o obj/individual.o

$(TARGET):$(OBJS)
		    $(CXX) $^ -o $@ $(CXXFLAGS) $(INCLUDE) 

obj/node.o:src/node.cc include/node.hh
			     $(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE) 
obj/pop.o:src/pop.cc include/pop.hh
			 $(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE) 
obj/gene.o:src/gene.cc include/gene.hh
			 $(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE) 
obj/individual.o:src/individual.cc include/individual.hh
			 $(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE) 
obj/sim.o:src/sim.cc 
			     $(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE) 
clean:
	  ${RM} $(OBJS) $(TARGET)
