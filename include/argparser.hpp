#pragma once

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace pr {

inline bool starts_with(const char *prefix, const char *arg)
{
  if (!memcmp(prefix, arg, strlen(prefix))) return true;
  else return false;
}

inline bool starts_with(char prefix, const char *arg)
{
  return arg[0] == prefix;
}

inline auto has_equalsign(const char *arg) -> const char* {
  for (char c=*arg; c!='\0'; c=*(++arg))
    if (c == '=') return ++arg;
  return nullptr;
}

using func = std::function<void(const char *)>;

struct Option {
  bool requires_value = false;
  const char *help_message = nullptr;
  const char *long_name    = nullptr;
  char short_name = '\0';
  func accept_value = nullptr;

  Option() = default;
  Option(bool rv, const char *help, const char *lname, char sname, func f)
      : requires_value(rv), help_message(help), long_name(lname),
        short_name(sname), accept_value(f) {}
};

struct Argument {
  const char *help_message = nullptr;
  const char *name  = nullptr;
  func accept_value = nullptr;

  Argument() = default;
  Argument(const char *help, const char *name, func f)
    : help_message(help), name(name), accept_value(f) {}
};


class ArgParser {
public:
  ArgParser() = default;
  void set_help_message(const char *message);
  void parse(const int argc, const char **argv);

  void add_option(Option &&);
  void add_option(bool &value, const char *help_msg, const char *long_name, char short_name);
  void add_option(int &value, const char *help_msg, const char *long_name, char short_name);
  void add_option(unsigned &value, const char *help_msg, const char *long_name, char short_name);
  void add_option(double &value, const char *help_msg, const char *long_name, char short_name);

  void add_argument(Argument &&);
  void add_argument(int &value, const char *help_msg, const char *name);
  void add_argument(double &value, const char *help_msg, const char *name);
  void add_argument(const char *&value, const char *help_msg, const char *name);

private:
  std::vector<Option> m_options;
  std::vector<Argument> m_positional_arguments;
  size_t m_curr_arg = 0;
};

inline void ArgParser::add_option(bool &value, const char *help_msg, const char *long_name, char short_name) {
  Option opt(false, help_msg, long_name, short_name, [&value](const char* arg=nullptr) { value = true; });
  m_options.push_back(std::move(opt));
}

inline void ArgParser::add_option(int &value, const char *help_msg, const char *long_name, char short_name)
{
  Option opt(true, help_msg, long_name, short_name, [&value](const char* arg) { value = std::stoi(arg); });
  m_options.push_back(std::move(opt));
}

inline void ArgParser::add_option(double &value, const char *help_msg, const char *long_name, char short_name)
{
  Option opt(true, help_msg, long_name, short_name, [&value](const char* arg) { value = std::stod(arg); });
  m_options.push_back(std::move(opt));
}

inline void ArgParser::add_argument(int &value, const char *help_msg, const char *name)
{
  Argument arg(help_msg, name, [&value](const char *parg) { value = std::stoi(parg); });
  m_positional_arguments.push_back(std::move(arg));
}

inline void ArgParser::add_argument(double &value, const char *help_msg, const char *name)
{
  Argument arg(help_msg, name, [&value](const char *parg) { value = std::stod(parg); });
  m_positional_arguments.push_back(std::move(arg));
}

inline void ArgParser::add_argument(const char *&value, const char *help_msg, const char *name)
{
  Argument arg(help_msg, name, [&value](const char *parg) { value = parg; });
  m_positional_arguments.push_back(std::move(arg));
}

inline void ArgParser::parse(const int argc, const char **argv)
{
  for (int i=1; i<argc; ++i) {
    // Long named options.
    if (starts_with("--", argv[i])) {
      for (auto itr = m_options.begin(); itr!=m_options.end(); ++itr)
	{
	  if (itr->long_name && starts_with(itr->long_name, argv[i]+2)) {
	    if (itr->requires_value) {
	      auto tmp = has_equalsign(argv[i]);
	      tmp ? itr->accept_value(tmp) : itr->accept_value(argv[++i]);
	    } else itr->accept_value(nullptr);
	    break;
	  }
	}
    } // Short named options.
    else if (starts_with('-', argv[i])) {
      for (auto itr = m_options.begin(); itr!=m_options.end(); ++itr)
	{
	  if (itr->short_name && starts_with(itr->short_name, argv[i]+1)) {
	    if (itr->requires_value) {
	      ( argv[i][2] != '\0' ) ? itr->accept_value(argv[i]+2) : itr->accept_value(argv[++i]);
	    } else itr->accept_value(nullptr);
	    break;
	  }
	}
    } // Positional arguments.
    else {
       if (m_curr_arg < m_positional_arguments.size()) {
	 m_positional_arguments.at(m_curr_arg++).accept_value(argv[i]);
       }
    }
  }
}
} // namespace pr
