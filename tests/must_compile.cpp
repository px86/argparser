#include "../include/argparser.hpp"

#include <array>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>

int main(int argc, char* argv[])
{
  // STEP 1 - Initialize static variables with default value
  static bool flag = false;
  static int key = 0;
  static const char* filepath = nullptr;

  // Declare the variable as std::array to accept fixed number of arguments
  static auto ratios = std::array<double, 3>{};

  // Declare the variable as std::vector to accept variable number of arguments
  // (args are pushed back at the end of the vector)
  static auto numbers_to_add = std::vector<int>{};

  // STEP 2 - Define the options into a static constexpr std::array
  static constexpr auto options = std::array{
    ap::opt("flag", 'f', "set the flag to true", &flag),
    ap::opt("key", 'k', "key value (int)", &key),
    ap::opt("path", 'p', "path to a file", &filepath),
    ap::opt("ratios", 0, "pass 3 doubles", &ratios),
    ap::opt("sum", 's', "pass integers to add (mark end of arguments with --)", &numbers_to_add),
  };

  // STEP 3 - Initialize and call the parse member function
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
