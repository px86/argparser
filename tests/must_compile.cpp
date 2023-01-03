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

    static constexpr auto options = std::array {
        pr::opt("flag", 'f', "set the flag to true", &flag),
        pr::opt("key", 'k', "key value (int)", &key),
        pr::opt("ratio", 'r', "ratio (double)", &ratio),
        pr::opt("file", 'p', "ratio (double)", &filepath),
    };

    pr::init<options>();

    std::cout << "Default Values\n"
              << "flag=" << flag << '\n'
              << "key=" << key << '\n'
              << "ratio=" << ratio << '\n'
              << "filepath=" << filepath << '\n'
              << std::endl;

    pr::parse<options>(argc, argv);

    std::cout << "After Parsing\n"
              << "flag=" << flag << '\n'
              << "key=" << key << '\n'
              << "ratio=" << ratio << '\n'
              << "filepath=" << filepath
              << std::endl;

    return EXIT_SUCCESS;
}
