CXX = g++
CXXFLAGS = -Wall -Werror -Wpedantic -std=c++17 -Iinclude

main: main.cpp include/argparser.hpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)

clean:
	rm -rf *.o main
