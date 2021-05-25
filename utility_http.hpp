#ifndef WEBSERV_UTILITY_HTTP_HPP
#define WEBSERV_UTILITY_HTTP_HPP

namespace http {

enum SPACE {
  SP,
  OWS,
  BWS,
  RWS
};

size_t get_token(std::string& dst, std::string const &data, size_t begin, int &err);
size_t get_request_target(std::string& dst, std::string const &data, size_t begin, int &err);
size_t get_http_version(std::string& dst, std::string const &data, size_t begin, int &err);
size_t skip_space(std::string const &data, size_t begin, SPACE, int &err);
size_t skip_crlf(std::string const &str, size_t begin, int &err);
size_t get_quoted_string(std::string& dst, std::string const &data, size_t begin, int &err);
bool istchar(int c);
bool isqdtext(int c);
void tolower(std::string &str);


}

#endif
