#include "http.hpp"
#include "error_http.hpp"
#include <cctype>
#include <cstring>

namespace http {

bool istchar(int c) {
  return (isalpha(c) || isdigit(c) || c == '!' || c == '#' || c == '$' ||
  c == '%' || c == '&' || c == '\'' || c == '*' || c == '+' || c == '-' ||
  c == '.' || c == '^' || c == '_' || c == '`' || c == '|' || c == '~');
}

void tolower(std::string &str) {
  std::string::iterator begin = str.begin();
  std::string::iterator end = str.end();
  for (; begin != end; ++begin)
    *begin = std::tolower(*begin);
}

}