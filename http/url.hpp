#ifndef WEBSERV_URL_HPP
#define WEBSERV_URL_HPP

#include <string>
#include <sys/types.h>

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
};

bool parse_origin_form(URL& url, std::string const& str);
bool parse_authority_form(URL& url, std::string const& str);
bool parse_asterisk_form(URL& url, std::string const& str);

bool parse_absolute_uri(URL& url, std::string const& str);
bool parse_partial_uri(URL& url, std::string const& str);

std::string	decodePCT(std::string const &str);
ssize_t get_scheme(std::string &scheme, std::string const &str, size_t begin);
size_t get_userinfo(std::string &userinfo, std::string const &str, size_t begin);
bool isIPliteral(std::string const &dst);
ssize_t get_host(std::string &dst, std::string const &str, size_t begin);
size_t get_port(std::string &port, std::string const &str, size_t begin);
bool isIPv4(std::string const& str);
ssize_t get_authority(URL &url, std::string const &str, size_t begin);

size_t get_abempty_path(std::string &path, std::string const &str, size_t begin);
ssize_t get_absolute_path(std::string &path, std::string const &str, size_t begin);
ssize_t get_noscheme_path(std::string &path, std::string const &str, size_t begin);
ssize_t get_rootless_path(std::string &path, std::string const &str, size_t begin);
ssize_t get_relative_part(URL &url, std::string const &str, size_t begin);

}
}
#endif
