TARGET=lib/libdiploid.so

CXX=g++
CXXFLAGS=-std=c++14 -g -ggdb -Wall -O3 -fPIC
INCLUDE=-I./include -I/usr/local/include
OBJS=obj/node.o obj/pop.o obj/gene.o obj/individual.o obj/popset.o obj/event.o obj/evlist.o obj/simulator.o

$(TARGET):$(OBJS)
#		    $(CXX) $^ -o $@ $(CXXFLAGS) $(INCLUDE) 
			 $(CXX) -fPIC -shared $^ -o $@

obj/node.o:src/node.cc include/node.hh
			     $(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE) 
obj/pop.o:src/pop.cc include/pop.hh
			 $(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE) 
obj/popset.o:src/popset.cc include/popset.hh
			    $(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE) 
obj/gene.o:src/gene.cc include/gene.hh
			 $(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE) 
obj/event.o:src/event.cc include/event.hh
			 $(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE) 
obj/evlist.o:src/evlist.cc include/evlist.hh
			 $(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE) 
obj/individual.o:src/individual.cc include/individual.hh
			 $(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE) 
obj/simulator.o:src/simulator.cc include/simulator.hh
			 $(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE) 
obj/sim.o:src/sim.cc 
			 $(CXX) $< -c -o $@ $(CXXFLAGS) $(INCLUDE) 
clean:
	  ${RM} $(OBJS) $(TARGET) 
