#ifndef WEBSERV_UTILITY_HTTP_HPP
#define WEBSERV_UTILITY_HTTP_HPP

namespace http {
	bool istchar(int c);
	message::e_method str_to_method(char *, size_t n);
}

#endif
