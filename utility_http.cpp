#include "http.hpp"
#include "error_http.hpp"
#include <cctype>

namespace http {
	bool	istchar(int c) {
		return (isalpha(c) || isdigit(c) || c == '!' || c ==  '#' || c ==  '$'
		|| c ==  '%' || c ==  '&' || c ==  '\'' || c ==  '*' || c ==  '+' ||
		c ==  '-' || c ==  '.' || c ==  '^' || c ==  '_' || c ==  '`' ||
		c ==  '|' || c ==  '~');
	}

	message::e_method str_to_method(char *, size_t n)
	{
		if (n > 5)
			error(501);
		message::e_method method = message::GET;
		return method;
	}
}