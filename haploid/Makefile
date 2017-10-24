TARGET1=sim
TARGET2=parallel

CXX=g++
CXXFLAGS=-std=c++14 -g -ggdb -Wall -O3
INCLUDE=-I./include -I/usr/local/include/nlohmann/
OBJS1=obj/node.o obj/sim.o obj/pop.o obj/gene.o obj/individual.o obj/popset.o
OBJS2=obj/node.o obj/parallel.o obj/pop.o obj/gene.o obj/individual.o obj/popset.o

all:
	 make $(TARGET1)
	 make $(TARGET2)

$(TARGET1):$(OBJS1)
		    $(CXX) $^ -o $@ $(CXXFLAGS) $(INCLUDE) 
$(TARGET2):$(OBJS2)
		    $(CXX) $^ -o $@ $(CXXFLAGS) $(INCLUDE) -pthread

obj/node.o:src/node.cc include/node.hh
			     $(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE) 
obj/pop.o:src/pop.cc include/pop.hh
			 $(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE) 
obj/popset.o:src/popset.cc include/popset.hh
			    $(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE) 
obj/gene.o:src/gene.cc include/gene.hh
			 $(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE) 
obj/individual.o:src/individual.cc include/individual.hh
			 $(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE) 
obj/sim.o:src/sim.cc 
			 $(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE) 
obj/parallel.o:src/parallel.cc 
			 $(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE) -pthread
clean:
	  ${RM} $(OBJS1) $(OBJS2) $(TARGET1) $(TARGET2) 
