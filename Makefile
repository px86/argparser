CXX = g++
CXXFLAGS = -Wall -Werror -Wpedantic -std=c++17 -Iinclude

test: test.cpp include/argparser.hpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)

clean:
	rm -rf *.o test
