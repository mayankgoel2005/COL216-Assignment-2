CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

all: forward noforward

forward: forward.cpp forward.hpp
	$(CXX) $(CXXFLAGS) -o forward forward.cpp

noforward: noforward.cpp noforward.hpp
	$(CXX) $(CXXFLAGS) -o noforward noforward.cpp

clean:
	rm -f forward noforward *.o

.PHONY: all clean