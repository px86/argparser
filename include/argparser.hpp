#ifndef PX86_ARGPARSER_HPP
#define PX86_ARGPARSER_HPP

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace ap {

namespace helper {

  constexpr bool strequal(const char* a, const char* b)
  {
    for (; (*a != 0) || (*b != 0);) {
      if (*a++ != *b++) {
        return false;
      }
    }
    return true;
  }

  constexpr auto const_strlen(const char* start) -> std::size_t
  {
    // NB: no nullptr checking!
    const char* end = start;
    for (; *end != '\0'; ++end) {
    }
    return (std::size_t)(end - start);
  }

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

  constexpr bool starts_with(char prefix, const char* arg) { return arg[0] == prefix; }

  constexpr bool match_long_name(const char* lname, const char* arg)
  {
    if (lname != nullptr && starts_with(lname, arg + 2)) {
      auto name_len = const_strlen(lname);
      if (arg[2 + name_len] == '=' || arg[2 + name_len] == '\0') {
        return true;
      }
    }
    return false;
  }

  inline constexpr bool match_short_name(char sname, const char* arg)
  {
    if (sname != '\0' && arg[1] != '\0') {
      return sname == arg[1];
    }
    return false;
  }

  // FIXME: Not used right now
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

}  // namespace helper

namespace value_parser {

  using VPFunc = char** (*)(void*, char**);
  using helper::starts_with;
  using helper::strequal;

  template <typename T>
  auto parse_single(const char* arg) -> T;

  template <>
  inline auto parse_single<bool>(const char* /*arg*/) -> bool
  {
    return true;
  }

  template <>
  auto parse_single<int>(const char* arg) -> int
  {
    try {
      if (starts_with("0x", arg)) {
        // hexadecimal
        return std::stoi(arg, nullptr, 16);
      } else if (starts_with('0', arg)) {
        // octal
        return std::stoi(arg, nullptr, 8);
      } else {
        // decimal
        return std::stoi(arg);
      }
    } catch (const std::invalid_argument& ex) {
      // TODO: throw custom exception, and don't exit here
      std::cerr << "invalid_argument: '" << arg << "' can not be parsed into int\n";
      std::exit(EXIT_FAILURE);
    } catch (const std::out_of_range& ex) {
      std::cerr << "out_of_range: '" << arg << "' can not fit into type int\n";
      std::exit(EXIT_FAILURE);
    }
  }

  template <>
  auto parse_single<double>(const char* arg) -> double
  {
    try {
      return std::stod(arg);
    } catch (const std::invalid_argument& ex) {
      // TODO: throw custom exception, and don't exit here
      std::cerr << "invalid_argument: '" << arg << "' can not be parsed into double\n";
      std::exit(EXIT_FAILURE);
    } catch (const std::out_of_range& ex) {
      std::cerr << "out_of_range: '" << arg << "' can not fit into type int\n";
      std::exit(EXIT_FAILURE);
    }
  }

  template <>
  inline auto parse_single<const char*>(const char* arg) -> const char*
  {
    // TODO: check for nullptr
    return arg;
  }

  template <typename T, std::size_t N>
  auto parse_n(void* dest, char** argv) -> char**
  {
    T* ptr = reinterpret_cast<T*>(dest);

    int values_parsed = 0;
    while (values_parsed != N) {
      if (*argv == nullptr) {
        // TODO: add some flexibility here
        throw std::invalid_argument("insufficient aguments provided");
      }
      *ptr = parse_single<T>(*argv);
      ++argv;
      ++ptr;
      ++values_parsed;
    }
    return argv;
  }

  template <>
  auto parse_n<bool, 0>(void* dest, char** argv) -> char**
  {
    *reinterpret_cast<bool*>(dest) = true;
    return argv;
  }

  template <typename T>
  auto parse_any(void* dest, char** argv) -> char**
  {
    // 'dest' must be a pointer to std::vector type
    auto ptr = reinterpret_cast<std::vector<T>*>(dest);

    while (*argv != nullptr) {
      if (strequal("--", *argv)) {
        return ++argv;
      }
      ptr->push_back(parse_single<T>(*argv));
      ++argv;
    }
    return argv;
  }

}  // namespace value_parser

namespace checks {

  using helper::strequal;
  using OptionNamePair = std::pair<const char*, char>;

  constexpr bool names_are_valid(const char* lname, char sname)
  {
    // return true if the non-nullptr/non-null names are valid
    // altest one of them must be valid
    if (lname != nullptr) {
      int i = 0;
      while (lname[i] != '\0') {
        switch (lname[i]) {
        case ' ':
        case '\t':
        case '\n':
        case '\f':
        case '\r':
          // long name contains whitespace
          return false;
        }
        ++i;
      }
      return true;
    } else if (sname != '\0') {
      // TODO: check if sname is a valid char
      return true;
    }

    // both names are invalid
    return false;
  }

  template <auto& OptionsArray>
  constexpr bool all_names_are_valid()
  {
    for (const auto& optn : OptionsArray) {
      if (!names_are_valid(optn.long_name, optn.short_name)) {
        return false;
      }
    }
    return true;
  }

  template <auto& OptionsArray>
  constexpr bool names_are_unique()
  {
    auto size = OptionsArray.size();
    const char* lname = nullptr;
    char sname = 0;

    for (auto i = 0UL; i < size; ++i) {
      lname = OptionsArray[i].long_name;
      sname = OptionsArray[i].short_name;

      for (auto j = i + 1; j < size; ++j) {
        if (lname != nullptr && strequal(lname, OptionsArray[j].long_name)) {
          return false;
        }
        if (sname != '\0' && sname == OptionsArray[j].short_name) {
          return false;
        }
      }
    }
    return true;
  }

  template <auto& OptionsArray>
  constexpr bool help_is_reserved()
  {
    for (const auto& optn : OptionsArray) {
      if (strequal("help", optn.long_name)) {
        return false;
      }
    }
    return true;
  }

}  // namespace checks

namespace impl {

  using value_parser::VPFunc;

  struct Option {
    const char* long_name{ nullptr };
    char short_name{ 0 };
    const char* help_str{ nullptr };
    void* ptr{ nullptr };
    VPFunc parser_func{ nullptr };
    int nargs{ 0 };

    auto parse(char** argv) const -> char** { return this->parser_func(this->ptr, argv); }

    constexpr Option() = default;
    constexpr Option(const char* lname, char sname, const char* help, void* ptr, VPFunc func, int nargs)
      : long_name(lname)
      , short_name(sname)
      , help_str(help)
      , ptr(ptr)
      , parser_func(func)
      , nargs(nargs)
    {}

    constexpr Option(const Option&) = default;
    constexpr Option(Option&&) = default;
    ~Option() = default;

    Option& operator=(const Option&) = default;
    Option& operator=(Option&&) = default;
  };

}  // namespace impl

namespace impl {

  namespace vp = value_parser;
  using checks::names_are_valid;

  template <typename T>
  constexpr Option opt(const char* lname, char sname, const char* help, T* dest)
  {
    if (!names_are_valid(lname, sname)) {
      throw std::invalid_argument("Option has invalid name(s)");
    }
    return Option(lname, sname, help, (void*)dest, vp::parse_n<T, 1>, 1);
  }

  template <>
  constexpr Option opt<bool>(const char* lname, char sname, const char* help, bool* dest)
  {
    if (!names_are_valid(lname, sname)) {
      throw std::invalid_argument("Option has invalid name(s)");
    }
    // bool always accepts zero arguments
    return Option(lname, sname, help, (void*)dest, vp::parse_n<bool, 0>, 0);
  }

  template <typename T, std::size_t N>
  constexpr Option opt(const char* lname, char sname, const char* help, std::array<T, N>* dest)
  {
    if (!names_are_valid(lname, sname)) {
      throw std::invalid_argument("Option has invalid name(s)");
    }
    return Option(lname, sname, help, (void*)dest, vp::parse_n<T, N>, N);
  }

  template <typename T>
  constexpr Option opt(const char* lname, char sname, const char* help, std::vector<T>* dest)
  {
    if (!names_are_valid(lname, sname)) {
      throw std::invalid_argument("Option has invalid name(s)");
    }
    return Option(lname, sname, help, (void*)dest, vp::parse_any<T>, -1);
  }

}  // namespace impl

namespace helper {

  // TODO: print better help messages, utilizing the nargs attribute
  template <auto& OptionsArray>
  void print_help_message()
  {
    constexpr int field_width = 20;
    std::cout.setf(std::ios_base::left, std::ios_base::adjustfield);

    std::cout << "Options:\n";
    for (auto& opt : OptionsArray) {
      std::ostringstream strm;
      //  For Example
      //  --option, -o [VAL]          help-message for option
      if (opt.short_name && opt.long_name) {
        strm << "  --" << opt.long_name << ", -" << opt.short_name;
      } else if (opt.long_name) {
        strm << "  --" << opt.long_name;
      } else if (opt.short_name) {
        strm << "  -" << opt.short_name;
      }

      if (opt.nargs == 1) {
        strm << " VAL";
      }

      auto opt_str = strm.str();

      // If the names are too long, then print the help message on the next line
      // with proper indentation.
      if (opt_str.size() >= field_width) {
        std::cout << std::setw(field_width) << opt_str << '\n';
        std::cout << std::setw(field_width) << ' ' << opt.help_str << '\n';
      } else {
        std::cout << std::setw(field_width) << opt_str << opt.help_str << '\n';
      }
    }
    std::cout.setf(std::ios_base::right, std::ios_base::adjustfield);
  }

  // FIXME: why did I do this?
  template <typename CharType>
  void print_unknow_option_message(CharType arg)
  {
    std::cout << "Error: unknown option '" << arg << "'" << std::endl;
    std::exit(1);
  }

}  // namespace helper

namespace impl {

  using checks::help_is_reserved;
  using checks::names_are_unique;
  using helper::match_long_name;
  using helper::match_short_name;
  using helper::print_help_message;
  using helper::print_unknow_option_message;
  using helper::strequal;

  template <auto& OptionsArray>
  struct argparser {

    static_assert(names_are_unique<OptionsArray>(), "Option names are not unique!");
    static_assert(help_is_reserved<OptionsArray>(), "Option name 'help' is reserved for help message!");

    static void parse(int /*argc*/, char** argv)
    {
      assert(argv != nullptr);
      ++argv;  // argv[0] is program name
      while (*argv != nullptr) {
        if (strequal("--help", *argv)) {
          print_help_message<OptionsArray>();
          std::exit(EXIT_SUCCESS);
        }
        bool optn_found = false;
        for (const auto& optn : OptionsArray) {
          if (match_long_name(optn.long_name, *argv) || match_short_name(optn.short_name, *argv)) {
            argv = optn.parse(++argv);
            optn_found = true;
            break;
          }
        }
        if (!optn_found) {
          // TODO: handle positional aguments
          print_unknow_option_message(*argv);
          std::exit(EXIT_FAILURE);
        }
      }
    }
  };

}  // namespace impl

using impl::argparser;
using impl::opt;

}  // namespace ap

#endif
