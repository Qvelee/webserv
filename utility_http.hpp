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

/*
 * URL
 */
void parse_origin_form(URL& url, const std::string& str);
void parse_authority_form(URL& url, const std::string& str);
void parse_asterisk_form(URL& url, const std::string& str);
bool isPctEncoded(std::string const &str, size_t pos);
std::string	decodePCT(const std::string& str);
size_t get_userinfo(std::string &dst, std::string const & src, size_t begin);
size_t get_ip_literal(std::string &dst, std::string const &str, size_t begin);
size_t get_host(std::string &dst, std::string const &str, size_t begin);
size_t get_port(std::string &dst, const std::string &str, size_t begin);
bool isIPv4(const std::string& str);
}

#endif
