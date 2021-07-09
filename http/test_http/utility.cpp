#include "../http.hpp"
#include "../test_http/utility.hpp"
#include <gtest/gtest.h>

namespace http {

std::ostream &
operator<<(std::ostream &os, const parameter &tp) {
  os << tp.name << "=" << tp.value;
  return os;
}

std::ostream &
operator<<(std::ostream &os, const transfer_extension &te) {
  os << te.token;
  for (auto &it : te.transfer_parameter) {
	os << ";" << it;
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, const TransferEncoding &r) {
  for (auto &it : r) {
	os << it << ", ";
  }
  os << "\n";
  return os;
}

std::ostream &operator<<(std::ostream &os, const Headers &r) {
  for (auto &it : r) {
	os << it.first << ":" << it.second << "\n";
  }
  return os;
}

namespace url {
std::ostream &operator<<(std::ostream &os, const url::URL &url) {
  os << url.userinfo << url.host << url.raw_path << url.raw_query;
  return os;
}
}

std::ostream &operator<<(std::ostream &os, const Request &r) {
  os << "Method: ";
  if (r.method == GET)
	os << "GET";
//  if (r.method == CONNECT)
//	os << "CONNECT";
//  if (r.method == HEAD)
//	os << "HEAD";
  if (r.method == POST)
	os << "POST";
//  if (r.method == PUT)
//	os << "PUT";
  if (r.method == DELETE)
	os << "DELETE";
//  if (r.method == OPTIONS)
//	os << "OPTIONS";
//  if (r.method == TRACE)
//	os << "TRACE";
  os << "\n";
  os << "URL: " << r.url << "\n";
  os << "Proto: " << r.proto << "\n";
  os << "Headers: " << r.headers << "\n";
  os << "Content-Length: " << r.content_length << "\n";
  os << "Transfer-Encoding: " << r.transfer_encoding << "\n";
  os << "Trailer: " << r.trailer << "\n";
  return os;
}

bool operator==(const parameter &lhs, const parameter &rhs) {
  if (lhs.name != rhs.name)
	return false;
  if (lhs.value != rhs.value)
	return false;
  return true;
}

bool operator==(const transfer_extension &lhs, const transfer_extension &rhs) {
  if (lhs.token != rhs.token)
	return false;
  if (!(lhs.transfer_parameter == rhs.transfer_parameter))
	return false;
  return true;
}

bool operator==(const media_type &lhs, const media_type &rhs) {
  if (lhs.type != rhs.type)
	return false;
  if (lhs.subtype != rhs.subtype)
	return false;
  if (!(lhs.parameters == rhs.parameters))
	return false;
  return true;
}

namespace url {
bool operator==(const url::URL &lhs, const url::URL &rhs) {
  if (lhs.scheme != rhs.scheme)
	return false;
//    if (opaque != rhs.opaque)
//	  return false;
  if (lhs.userinfo != rhs.userinfo)
	return false;
  if (lhs.host != rhs.host)
	return false;
  if (lhs.path != rhs.path)
	return false;
  if (lhs.raw_path != rhs.raw_path)
	return false;
  if (lhs.raw_query != rhs.raw_query)
	return false;
//	if (fragment != rhs.fragment)
//	  return false;
//	if (raw_fragment != rhs.raw_fragment)
//	  return false;
  return true;
}
}

bool operator==(const Request &lhs, const Request &rhs) {
  if (lhs.method != rhs.method)
	return false;
  if (!(lhs.url == rhs.url))
	return false;
  if (lhs.proto != rhs.proto)
	return false;
  if (lhs.headers != rhs.headers)
	return false;
  if (lhs.content_length != rhs.content_length)
	return false;
  if (lhs.transfer_encoding != rhs.transfer_encoding)
	return false;
  if (lhs.trailer != rhs.trailer)
	return false;
  return true;
}

}