#ifndef WEBSERV__URL_HPP_
#define WEBSERV__URL_HPP_

#include <string>

namespace http {
namespace url {

struct URL {
  std::string scheme;
//  std::string opaque;
  std::string userinfo;
  std::string host;
  std::string path;
  std::string raw_path;
  std::string raw_query;
//  std::string fragment;
//  std::string raw_fragment;

  // for test
  bool operator==(const http::url::URL &rhs) const;
};

void parse_origin_form(URL& url, const std::string& str);
void parse_authority_form(URL& url, const std::string& str);
void parse_asterisk_form(URL& url, const std::string& str);
void parse_absolute_uri(URL& url, const std::string& str);
void parse_partial_uri(URL& url, const std::string& str);
bool isPctEncoded(std::string const &str, size_t pos);
std::string	decodePCT(const std::string& str);
size_t get_userinfo(std::string &userinfo, std::string const & str, size_t begin);
size_t get_ip_literal(std::string &dst, std::string const &str, size_t begin);
size_t get_host(std::string &dst, std::string const &str, size_t begin);
size_t get_port(std::string &port, const std::string &str, size_t begin);
bool isIPv4(const std::string& str);
}

}
#endif
