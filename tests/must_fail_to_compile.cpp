#include "../include/argparser.hpp"

#include <array>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>

int main(int argc, char* argv[])
{
  static bool flag = false;
  static int key = 0;
  static const char* filepath = nullptr;
  static auto ratios = std::array<double, 3>{};
  static auto numbers_to_add = std::vector<int>{};

  static constexpr auto options = std::array{
    ap::opt("fla  g", 'f', "set the flag to true", &flag),  // ERROR: whitespace in name
    ap::opt(nullptr, 0, "key value (int)", &key),           // ERROR: both names are invalid
    ap::opt("path", 'k', "path to a file", &filepath),      // ERROR: duplicate short name
    ap::opt("help", 0, "pass 3 doubles", &ratios),          // ERROR: help name is reserved
    ap::opt("sum", 's', "pass integers to add (mark end of arguments with --)", &numbers_to_add),
  };

  ap::argparser<options>::parse(argc, argv);

  std::cout << "flag = " << std::boolalpha << flag << '\n';
  std::cout << "key = " << key << '\n';

  std::cout << "ratios = { ";
  for (auto x : ratios) {
    std::cout << x << " ";
  }
  std::cout << "}\n";

  std::cout << "numbers_to_add = { ";
  for (auto x : numbers_to_add) {
    std::cout << x << " ";
  }
  std::cout << "}\n";

  std::cout << "sum is " << std::accumulate(numbers_to_add.cbegin(), numbers_to_add.cend(), 0) << std::endl;

  return EXIT_SUCCESS;
}
