#ifndef WEBSERV_TEST_HTTP_UTILITY_HPP_
#define WEBSERV_TEST_HTTP_UTILITY_HPP_

#include "../http.hpp"
#include "../url.hpp"
namespace http {
bool operator==(const http::Request &lhs, const http::Request &rhs);
bool operator==(const http::parameter &lhs, const http::parameter &rhs);
bool operator==(const http::transfer_extension &lhs, const http::transfer_extension &rhs);
bool operator==(const http::media_type &lhs, const http::media_type &rhs);
// for test
std::ostream &operator<<(std::ostream &os, const Headers &r);
std::ostream &operator<<(std::ostream &os, const TransferEncoding &r);
std::ostream &operator<<(std::ostream &os, const Request &r);
std::ostream &operator<<(std::ostream &, const parameter &);
std::ostream &operator<<(std::ostream &, const transfer_extension &);
namespace url {
bool operator==(const url::URL &lhs, const url::URL &rhs);
std::ostream &operator<<(std::ostream &, const url::URL &);
}
}
#endif
