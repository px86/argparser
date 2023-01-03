#include "../include/argparser.hpp"
#include <array>
#include <cstdlib>
#include <iostream>

// This source file must fail to compile.
int main(int argc, char** argv)
{
    // Initialize static variables with default value.
    static bool flag = false;
    static int key = 0;
    static double ratio = 0.0;
    static const char* filepath = "";

    static constexpr auto options = std::array {
        pr::opt("fla\tg\n", 'f', "set the flag to true", &flag), // whitespace in name
        pr::opt("key\r ", 'k', "key value (int)", &key), // whitespace in name
        pr::opt("rat io", 'r', "ratio (double)", &ratio), // whitespace in name
        pr::opt("file", 'h', "ratio (double)", &filepath), // h is reserved for help option
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