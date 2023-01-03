#include "../include/argparser.hpp"
#include <array>
#include <cstdlib>
#include <iostream>

int main(int argc, char** argv)
{
  // Initialize static variables with default value.
  static bool flag = false;
  static int key = 0;
  static double ratio = 0.0;
  static const char* filepath = "";

  static constexpr auto options = std::array{
    ap::opt("flag", 'f', "set the flag to true", &flag),
    ap::opt("key", 'k', "key value (int)", &key),
    ap::opt("ratio", 'r', "ratio (double)", &ratio),
    ap::opt("file", 'p', "path to a file", &filepath),
  };

  ap::init<options>();

  std::cout << "Default Values\n"
            << "flag=" << flag << '\n'
            << "key=" << key << '\n'
            << "ratio=" << ratio << '\n'
            << "filepath=" << filepath << '\n'
            << std::endl;

  ap::parse<options>(argc, argv);

  std::cout << "After Parsing\n"
            << "flag=" << flag << '\n'
            << "key=" << key << '\n'
            << "ratio=" << ratio << '\n'
            << "filepath=" << filepath << std::endl;

  return EXIT_SUCCESS;
}
