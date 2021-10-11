#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace PR {

using namespace std;

class ArgParse {
public:
  struct Option {
    bool requires_argument { false };
    const char *help_message { nullptr };
    const char *long_name { nullptr };
    char short_name { 0 };
    function<bool(const char *)> accept_value { nullptr };
  };

  struct Argument {};

  ArgParse();
  void set_help_message(const char *message);
  void parse(const int argc, const char *argv[]);

  void add_option(Option &&);
  void add_option(bool &value, const char *help_msg, const char *long_name, char short_name);
  void add_option(int &value, const char *help_msg, const char *long_name, char short_name);
  void add_option(unsigned &value, const char *help_msg, const char *long_name, char short_name);
  void add_option(double &value, const char *help_msg, const char *long_name, char short_name);

  void add_positional_argument(Argument &&);

private:
  vector<Option> m_options;
  vector<Argument> m_positional_arguments;
};

inline ArgParse::ArgParse() {}

inline void ArgParse::add_option(int &value,
				 const char *help_msg,
                                 const char *long_name,
				 char short_name = 0)
{
  Option option {
    true,
    help_msg,
    long_name,
    short_name,
    [&value](const char *arg) {
      char* p;
      int v = strtol(arg, &p, 10);
      if (p==arg) return false;
      value = v; 
      return true;
    }
  };
  m_options.push_back(move(option));
}

inline void ArgParse::add_option(double &value,
				 const char *help_msg,
                                 const char *long_name,
				 char short_name = 0)
{
  Option option{
    true,
    help_msg,
    long_name,
    short_name,
    [&value](const char *arg) {
      char* p;
      double v = strtod(arg, &p);
      if (p==arg) return false;
      value = v;
      return true;
    }
  };
  m_options.push_back(move(option));
}

inline void ArgParse::add_option(bool &value,
				 const char *help_msg,
                                 const char *long_name,
				 char short_name = 0)
{
  Option option {
    false,
    help_msg,
    long_name,
    short_name,
    [&value](const char *arg=nullptr) {
      return value = true;
    }
  };
  m_options.push_back(move(option));
}

inline void ArgParse::parse(const int argc, const char **argv) {

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      // arg is an option
      Option* opt { nullptr };
      const char* val_for_parsing { nullptr };

      if (argv[i][1] == '-') {
	// long option
	size_t last_index = 1;
	char last_char = argv[i][last_index];
	while (last_char!='\0' && last_char!='=') last_char = argv[i][++last_index];
	
        for (int j = 0; j < (int)m_options.size(); ++j) {
          if (!memcmp(m_options[j].long_name, argv[i]+2, last_index-2)) {
            opt = &m_options[j];
	    val_for_parsing = last_char=='=' ? argv[i] + last_index + 1 : argv[++i];
            break;
          }
        }
      }
      else {
	// short option
        for (int j = 0; j < (int)m_options.size(); ++j) {
          if (m_options[j].short_name == argv[i][1]) {
            opt = &m_options[j];
	    val_for_parsing = argv[i][2] ? argv[i] + 2 : argv[++i];
            break;
          }
        }
      }
      if (!opt) { cerr << "Unknown option: " << argv[i] << endl; }
      else if (opt->requires_argument) opt->accept_value(val_for_parsing);
      else opt->accept_value(nullptr);
    }
  }
}

} // namespace PR

#endif
