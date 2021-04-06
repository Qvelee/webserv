#include "http.hpp"
#include "error_http.hpp"
#include <cctype>

namespace http {
	bool istchar(int c)
	{
		return (isalpha(c) || isdigit(c) || c == '!' || c == '#' || c == '$'
				|| c == '%' || c == '&' || c == '\'' || c == '*' || c == '+' ||
				c == '-' || c == '.' || c == '^' || c == '_' || c == '`' ||
				c == '|' || c == '~');
	}
}