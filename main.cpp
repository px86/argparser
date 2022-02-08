#include <iostream>
#include "include/argparser.hpp"

using namespace std;

int main(int argc, const char** argv) {

  // Variables initialized with default values.
  bool var_bool = false;
  double var_double = 0.0;
  int var_int = -1;
  const char *var_charpointer = "";
  int parg_int = -100;

  // Initialize argument parser.
  pr::ArgParser ap;

  ap.add_option(var_bool, "Switch the bool flag", "bool", 'b');
  ap.add_option(var_int, "Value for int", "int", 'i');
  ap.add_option(var_double, "Value for double", "double", 'd');

  ap.add_argument(var_charpointer, "Provide file path", "<filepath>");
  ap.add_argument(parg_int, "Integer argument", "<integer>");

  // Parse the arguments.
  ap.parse(argc, argv);

  std::cout << "BOOL " << var_bool << '\n'
	    << "INT "  << var_int << '\n'
	    << "DOUBLE "  << var_double << '\n'
	    << "Path1 "   << var_charpointer << '\n'
    	    << "Arg Int " << parg_int << '\n';

  return 0;
}
