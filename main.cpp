
#include "include/ArgParse.hpp"
#include <iostream>

using namespace std;


int main(int argc, const char** argv) {
  // initialize with default values
  int key = -1;
  bool flag = false;
  double ratio = -1;

  PR::ArgParse argparse;

  argparse.add_option(key, "Pass secret key as integer", "key", 'k');
  argparse.add_option(flag, "Flag bit", "flag", 'f');
  argparse.add_option(ratio, "Pass ratio as double", "ratio", 'r');

  argparse.parse(argc, argv);

  cout << "Flag  is: " << flag << endl;
  cout << "Ratio is: " << ratio << endl;
  cout << "Secret key is: " << key << endl;

  return 0;
}
