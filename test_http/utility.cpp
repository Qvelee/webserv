#include "../http.hpp"
#include <gtest/gtest.h>

std::ostream& operator<<(std::ostream& os, const http::Request& r) {
  os << "Method: ";
  if (r.method == http::GET)
	os << "GET";
  if (r.method == http::CONNECT)
	os << "CONNECT";
  if (r.method == http::HEAD)
	os << "HEAD";
  if (r.method == http::POST)
	os << "POST";
  if (r.method == http::PUT)
	os << "PUT";
  if (r.method == http::DELETE)
	os << "DELETE";
  if (r.method == http::OPTIONS)
	os << "OPTIONS";
  if (r.method == http::TRACE)
	os << "TRACE";
  os << "\n";
  os << "URL: " << r.url << "\n";
  os << "Proto: " << r.proto << "\n";
  os << "Headers: " << r.headers << "\n";
  os << "Content-Length: " << r.content_length << "\n";
  os << "Transfer-Encoding: " << r.transfer_encoding << "\n";
  os << "Trailer: " << r.trailer << "\n";
  return os;
}

std::ostream& operator<<(std::ostream& os, const http::TransferEncoding & r) {
  for (auto &it : r) {
	os << it << ", ";
  }
  os << "\n";
  return os;
}

std::ostream& operator<<(std::ostream& os, const http::Headers& r) {
  for (auto &it : r) {
	os << it.first << ":" << it.second << "\n";
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const http::url::URL& url) {
  os << url.userinfo << url.host << url.raw_path << url.raw_query;
  return os;
}



std::ostream&
operator<<(std::ostream& os, const http::parameter& tp) {
  os << tp.name << "=" << tp.value;
  return os;
}

std::ostream&
operator<<(std::ostream& os, const http::transfer_extension& te) {
  os << te.token;
  for (auto &it : te.transfer_parameter) {
	os << ";" << it;
  }
  return os;
}

bool http::parameter::operator==(const http::parameter& rhs) const {
  if (name != rhs.name)
	return false;
  if (value != rhs.value)
	return false;
  return true;
}

bool http::transfer_extension::operator==(const http::transfer_extension& rhs) const {
  if (token != rhs.token)
	return false;
  if (!(transfer_parameter == rhs.transfer_parameter))
	return false;
  return true;
}

bool http::media_type::operator==(const http::media_type& rhs) const {
  if (type != rhs.type)
	return false;
  if (subtype != rhs.subtype)
	return false;
  if (!(parameters == rhs.parameters))
	return false;
  return true;
}

http::Request::Request(Method m, url::URL url, std::string proto, Headers headers,
	int64_t content_length, TransferEncoding te,
	Headers t, std::string body, representation_metadata meta)
: method(m),
url(std::move(url)),
proto(std::move(proto)),
headers(std::move(headers)),
content_length(content_length),
transfer_encoding(std::move(te)),
trailer(std::move(t)),
body(std::move(body)),
metadata(std::move(meta)) { }

bool http::Request::operator==(const http::Request &rhs) const {
  if (method != rhs.method)
	return false;
  if (!(url == rhs.url))
	return false;
  if (proto != rhs.proto)
	return false;
  if (headers != rhs.headers)
	return false;
  if (content_length != rhs.content_length)
	return false;
  if (transfer_encoding != rhs.transfer_encoding)
	return false;
  if (trailer != rhs.trailer)
	return false;
  return true;
}

bool http::url::URL::operator==(const http::url::URL& rhs) const {
//    if (scheme != rhs.scheme)
//	  return false;
//    if (opaque != rhs.opaque)
//	  return false;
  if (userinfo != rhs.userinfo)
	return false;
  if (host != rhs.host)
	return false;
  if (path != rhs.path)
	return false;
  if (raw_path != rhs.raw_path)
	return false;
  if (raw_query != rhs.raw_query)
	return false;
//	if (fragment != rhs.fragment)
//	  return false;
//	if (raw_fragment != rhs.raw_fragment)
//	  return false;
  return true;
}