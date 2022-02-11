#include "include/argparser.hpp"

#include <ios>
#include <iostream>

using namespace std;

int main(int argc, char **argv) {

  // Variables initialized with default values.
  int key = -1;
  bool flag = false;

  // Initialize argument parser with program name.
  auto ap = pr::ArgParser("example");

  // Add options
  ap.add_option(flag, "Turn on flag", "flag", 'f');
  ap.add_option(key, "Value for key", "key", 'k');

  ap.parse(argc, argv);

  std::cout << "After parsing the arguments:\n"
	    << "key: " << key << '\n'
	    << "flag: " << std::boolalpha << flag << std::endl;

  return 0;
}
