CXX = g++
CXXFLAGS = --std=c++14 -Wall -Wextra -Werror -Wno-unused-parameter

all: main

main: main.cpp include/ArgParse.hpp
	$(CXX) $(CXXFLAGS) -o $@ main.cpp

clean:
	rm -f main
