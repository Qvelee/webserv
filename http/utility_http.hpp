#ifndef WEBSERV_UTILITY_HTTP_HPP
#define WEBSERV_UTILITY_HTTP_HPP

namespace http {

enum SPACE {
  SP,
  OWS,
  BWS,
  RWS
};

std::size_t get_token(std::string& dst, const char *src);
std::size_t get_request_target(std::string& dst, const char *src);
std::size_t get_http_version(std::string& dst, const char *src);
std::size_t skip_space(const char *src, SPACE);
std::size_t skip_crlf(const char *src);
std::size_t get_quoted_string(std::string& dst, const char *src);
bool istchar(int c);
bool isqdtext(int c);
void tolower(std::string &str);

}

#endif
