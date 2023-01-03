#pragma once

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>

namespace pr {

namespace helper {

    // descriptive error codes
    enum class ErrorCode : int {
        MULTIPLE_OPTIONS_HAVE_SAME_LONG_NAME = 1,
        MULTIPLE_OPTIONS_HAVE_SAME_SHORT_NAME,
        NAME_RESERVED_FOR_HELP_OPTION,
        WHITESPACE_IN_LONG_NAME,
        BOTH_NAMES_ARE_INVALID
    };

    constexpr bool starts_with(const char* prefix, const char* arg)
    {
        if (prefix == nullptr || arg == nullptr) {
            return false;
        }
        int i = 0;
        while (prefix[i] != '\0') {
            if (prefix[i] != arg[i]) {
                return false;
            }
            ++i;
        }
        return true;
    }

    constexpr bool starts_with(char prefix, const char* arg)
    {
        return arg[0] == prefix;
    }

    constexpr bool match_long_name(const char* lname, const char* arg)
    {
        if (lname != nullptr && starts_with(lname, arg + 2)) {
            auto name_len = std::strlen(lname);
            if (arg[2 + name_len] == '=' || arg[2 + name_len] == '\0') {
                return true;
            }
        }
        return false;
    }

    constexpr bool names_are_valid(const char* lname, char sname = '\0')
    {
        // return true if lname is valid. If lname is nullptr, check
        // if sname is valid. If none of these conditions are statisfied
        // call std::exit(1) and terminate the program.

        if (lname != nullptr) {
            int i = 0;
            while (lname[i] != '\0') {
                switch (lname[i]) {
                case ' ':
                case '\t':
                case '\n':
                case '\f':
                case '\r':
                    std::exit((int)ErrorCode::WHITESPACE_IN_LONG_NAME); // Error: option name contains whitespace
                }
                ++i;
            }
            return true;
        } else if (sname != '\0') {
            return true;
        }

        std::exit((int)ErrorCode::BOTH_NAMES_ARE_INVALID); // Error: invalid option names
    }

    constexpr auto has_equalsign(const char* arg) -> const char*
    {
        // return pointer to the (non-null) char right after the (first) '=' char,
        // if no '=' is present, or if it is the last char in the str, return nullptr
        if (arg == nullptr) {
            return nullptr;
        }
        int i = 0;
        while (arg[i] != '\0') {
            if (arg[i] == '=') {
                if (arg[++i] == '\0') {
                    // CASE: --key=
                    return nullptr;
                }
                return arg + i;
            }
            ++i;
        }
        return nullptr;
    }

} // namespace helper

namespace value_parser {

    void vp_bool(void* ptr, const char*)
    {
        *reinterpret_cast<bool*>(ptr) = true;
    }

    void vp_int(void* ptr, const char* arg)
    {
        try {
            if (helper::starts_with("0x", arg)) {
                // hexadecimal integer, set base as 16
                *reinterpret_cast<int*>(ptr) = std::stoi(arg, 0, 16);
            } else if (helper::starts_with("0", arg)) {
                // octal integer, set base as 8
                *reinterpret_cast<int*>(ptr) = std::stoi(arg, 0, 8);
            } else {
                // decimal integer, base is 10
                *reinterpret_cast<int*>(ptr) = std::stoi(arg);
            }

        } catch (const std::invalid_argument& ex) {
            std::cerr << "invalid_argument: '" << arg << "' can not be parsed into int\n";
            std::exit(EXIT_FAILURE);
        } catch (const std::out_of_range& ex) {
            std::cerr << "out_of_range: '" << arg << "' can not fit into type int\n";
            std::exit(EXIT_FAILURE);
        }
    }

    void vp_double(void* ptr, const char* arg)
    {
        try {
            *reinterpret_cast<double*>(ptr) = std::stod(arg);
        } catch (const std::invalid_argument& ex) {
            std::cerr << "invalid_argument: '" << arg << "' can not be parsed into double\n";
            std::exit(EXIT_FAILURE);
        } catch (const std::out_of_range& ex) {
            std::cerr << "out_of_range: '" << arg << "' can not fit into type int\n";
            std::exit(EXIT_FAILURE);
        }
    }

    void vp_const_char_ptr(void* ptr, const char* arg)
    {
        *reinterpret_cast<const char**>(ptr) = arg;
    }

} // namespace value_parser

namespace impl {

    // function pointer to value parser functions
    typedef void (*VPFunc)(void*, const char*);

    struct Option {
        const char* long_name { nullptr };
        char short_name { 0 };
        const char* help_str { nullptr };
        void* ptr { nullptr };
        VPFunc parser_func { nullptr };
        int nargs { 0 };

        void parse(const char* arg) const { this->parser_func(this->ptr, arg); }

        constexpr Option() = default;
        constexpr Option(const char* lname, char sname, const char* help, void* ptr, VPFunc func, int nargs)
            : long_name(lname)
            , short_name(sname)
            , help_str(help)
            , ptr(ptr)
            , parser_func(func)
            , nargs(nargs)
        {
            helper::names_are_valid(lname, sname); // calls std::exit if names are invalid
        }

        constexpr Option(const Option&) = default;
        constexpr Option(Option&&) = default;

        Option& operator=(const Option&) = default;
        Option& operator=(Option&&) = default;
    };

    template <auto& OptionsArray>
    void parse(int argc, char* argv[])
    {
        using helper::starts_with;

        for (int argv_indx = 1; argv_indx < argc; ++argv_indx) {
            const char* arg = argv[argv_indx];

            // long options, --key=val, --key val
            if (starts_with("--", arg)) {
                for (const auto& optn : OptionsArray) {
                    if (!helper::match_long_name(optn.long_name, arg)) {
                        continue;
                    }
                    if (optn.nargs) {
                        auto t = helper::has_equalsign(arg);
                        t != nullptr ? optn.parse(t) : optn.parse(argv[++argv_indx]);
                    } else {
                        optn.parse(nullptr);
                    }
                    break;
                }
            }
            // short option, -k=val, -k val
            else if (starts_with('-', arg)) {
                for (const auto& optn : OptionsArray) {
                    if (optn.short_name && starts_with(optn.short_name, arg + 1)) {
                        if (optn.nargs) {
                            arg[2] == '=' ? optn.parse(arg + 3) : optn.parse(argv[++argv_indx]);
                        } else {
                            optn.parse(nullptr);
                        }
                        break;
                    }
                }
            } else {
                // Positional argument (not implemented yet!)
            }
        }
    }

} // namespace impl

namespace helper {

    constexpr bool strequal(const char* a, const char* b)
    {
        for (; *a || *b;) {
            if (*a++ != *b++)
                return false;
        }
        return true;
    }

    template <auto& OptionsArray>
    constexpr bool names_are_unique()
    {
        auto opts = OptionsArray.data();
        auto size = OptionsArray.size();
        const char* lname = nullptr;
        char sname = 0;
        for (auto i = 0UL; i < size; ++i) {
            lname = opts[i].long_name;
            sname = opts[i].short_name;
            for (auto j = i + 1; j < size; ++j) {
                if (strequal(opts[j].long_name, lname)) {
                    std::exit((int)ErrorCode::MULTIPLE_OPTIONS_HAVE_SAME_LONG_NAME); // Error: Two options have the same long names.
                } else if (opts[j].short_name == sname) {
                    std::exit((int)ErrorCode::MULTIPLE_OPTIONS_HAVE_SAME_SHORT_NAME); // Error: Two options have the same short names.
                }
            }
            if (sname == 'h') {
                std::exit((int)ErrorCode::NAME_RESERVED_FOR_HELP_OPTION); // Error: --help and -h are reserved for help options
            }
        }
        return true;
    }

} //namespace helper

namespace impl {
    template <typename T>
    constexpr Option opt(const char*, char, const char*, T*, int = 1);

    template <>
    constexpr Option opt<int>(const char* lname, char sname, const char* help, int* val, int nargs)
    {
        return Option(lname, sname, help, val, value_parser::vp_int, nargs);
    }

    template <>
    constexpr Option opt<bool>(const char* lname, char sname, const char* help, bool* val, int)
    {
        // nargs for bool options will always be 0.
        return Option(lname, sname, help, val, value_parser::vp_bool, 0);
    }

    template <>
    constexpr Option opt<double>(const char* lname, char sname, const char* help, double* val, int nargs)
    {
        return Option(lname, sname, help, val, pr::value_parser::vp_double, nargs);
    }

    template <>
    constexpr Option opt<const char*>(const char* lname, char sname, const char* help, const char** val, int nargs)
    {
        return Option(lname, sname, help, val, pr::value_parser::vp_const_char_ptr, nargs);
    }

} // namespace impl

namespace helper {

    // force computation to be done at compile time
    template <bool B>
    struct force_compute {
        static constexpr bool value = B;
    };

    template <auto& OptionsArray>
    constexpr bool init()
    {
        // verify if all options have unique names
        return force_compute<names_are_unique<OptionsArray>()>::value;
    }

} // namespace helper

using helper::init;
using impl::opt;
using impl::parse;

} // namespace pr
