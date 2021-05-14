#ifndef WEBSERV_HTTP_HPP
#define WEBSERV_HTTP_HPP

#include <string>
#include <utility>
#include <vector>
#include <map>
#include <iostream>

namespace http {

enum Method {
  GET,
  HEAD,
  POST,
  PUT,
  DELETE,
  CONNECT,
  OPTIONS,
  TRACE
};

//struct URL {
//
//};

struct transfer_parameter {
  std::string name;
  std::string value;

  bool operator==(const http::transfer_parameter& rhs) const {
	if (name != rhs.name)
	  return false;
	if (value != rhs.value)
	  return false;
	return true;
  }
};

struct transfer_extension {
  std::string token;
  std::vector<transfer_parameter> transfer_parameter;

  bool operator==(const http::transfer_extension& rhs) const {
	if (token != rhs.token)
	  return false;
	if (!(transfer_parameter == rhs.transfer_parameter))
	  return false;
	return true;
  }
};

typedef std::map<std::string, std::string>		Headers;
typedef std::vector<transfer_extension>			TransferEncoding;

typedef std::string URL;

struct Request {
  Method			method;
  URL				url;
  std::string		proto;
  Headers			headers;
  int64_t			content_length;
  TransferEncoding	transfer_encoding;
  Headers			trailer;

  Request(Method m=GET, URL url="", std::string proto="", Headers headers=Headers{},
		  int64_t content_length=-1, TransferEncoding te=TransferEncoding{},
		  Headers t = Headers{})
		  : method(m),
		  url(std::move(url)),
		  proto(std::move(proto)),
		  headers(std::move(headers)),
		  content_length(content_length),
		  transfer_encoding(std::move(te)),
		  trailer(std::move(t)) {
  }

  bool operator==(const http::Request &rhs) const {
    if (method != rhs.method)
      return false;
    if (url != rhs.url)
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
};

typedef const std::map<std::string, void (*)(Request& req, std::string const &field)>
    HeaderHandlers;
typedef const std::map<std::string, int> TransferCodingRegister;

bool					parse_request(Request& req, const char *data);
std::size_t				parse_request_line(Request &r, const char *bytes);
std::size_t 			parse_headers(Headers& dst, const char *bytes);
void					header_analysis(Request& r, Headers &h);
HeaderHandlers& 		get_header_field_handlers();
void					transfer_encoding(Request& req, std::string const &value);
TransferCodingRegister&	get_transfer_coding_register();
void					validate_transfer_coding(const std::string& name);
void					content_length(Request& req, std::string const &value);
void					calculate_length_message(Request& req);



struct Response {
};




//void read_message_body(const char *bytes);
//
//static std::size_t read_chunk_size(std::size_t& size, const char *bytes);
//void decoding_chunked(const char *bytes);
//
//static void	parse_request_target(URL &url, std::string const &src);
//static void	parse_and_validate_method(Method &m, std::string const &src);
//
//


}
std::ostream& operator<<(std::ostream& os, const http::Headers& r);
std::ostream& operator<<(std::ostream& os, const http::TransferEncoding & r);
std::ostream& operator<<(std::ostream& os, const http::Request& r);
std::ostream& operator<<(std::ostream&, const http::transfer_parameter&);
std::ostream& operator<<(std::ostream&, const http::transfer_extension&);


#endif
