#ifndef WEBSERV_HTTP_HPP
#define WEBSERV_HTTP_HPP

#include <string>
#include <utility>
#include <vector>
#include <map>
#include <iostream>
#include "url.hpp"

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

struct parameter {
  std::string name;
  std::string value;

  // for test
  bool operator==(const http::parameter& rhs) const;
};

struct transfer_extension {
  std::string token;
  std::vector<parameter> transfer_parameter;

  // for test
  bool operator==(const http::transfer_extension& rhs) const;
};


struct media_type {
  std::string type;
  std::string subtype;
  std::vector<parameter> parameters;

  // for test
  bool operator==(const http::media_type& rhs) const;
};

struct representation_metadata {
  media_type				media_type;
  std::vector<std::string>	content_language;
  url::URL						content_location;
};

typedef std::map<std::string, std::string>		Headers;
typedef std::vector<transfer_extension>			TransferEncoding;

struct Request {
  Method					method;
  url::URL						url;
  std::string				proto;
  Headers					headers;
  int64_t					content_length;
  TransferEncoding			transfer_encoding;
  Headers					trailer;
  std::string				body;
  representation_metadata	metadata;


  // for tests
  Request(Method m=GET, url::URL url={}, std::string proto="", Headers headers=Headers{},
		  int64_t content_length=-1, TransferEncoding te=TransferEncoding{},
		  Headers t = Headers{}, std::string body="", representation_metadata
		  meta=representation_metadata{});
  bool operator==(const http::Request &rhs) const;
};

typedef const std::map<std::string, void (*)(Request& req, std::string const &field)>
    HeaderHandlers;
typedef const std::map<std::string, int> TransferCodingRegister;
typedef const std::map<std::string, int> MediaTypeRegister;

bool			parse_request(Request& req, std::string const &data);
size_t			parse_request_line(Request &r, std::string const &data, size_t begin);
void			parse_and_validate_method(Method &m, std::string const &src);
void			parse_request_target(url::URL &url, Method &m, std::string const &src);
size_t 			parse_headers(Headers& dst, std::string const& data, size_t begin);
void			header_analysis(Request& r, Headers &h);
HeaderHandlers&	get_header_field_handlers();
//header_field_handlers
void					transfer_encoding(Request& req, std::string const &value);
TransferCodingRegister&	get_transfer_coding_register();
void					validate_transfer_coding(const std::string& name);
void					content_length(Request& req, std::string const &value);
void					host(Request& req, std::string const &value);
void 					content_type(Request &req, std::string const &value);
MediaTypeRegister&		get_media_type_register();
void					validate_media_type(const media_type& type);
void					content_language(Request &req, std::string const &value);
void					content_location(Request &req, std::string const &value);

void	calculate_length_message(Request& req);
bool	read_body(Request &req, std::string const &data, size_t begin);
bool	decoding_chunked(Request& req, std::string const &data, size_t begin);
size_t	read_chunk_size(size_t& size, std::string const &data, size_t begin);


//struct Response {
//};

}

std::ostream& operator<<(std::ostream& os, const http::Headers& r);
std::ostream& operator<<(std::ostream& os, const http::TransferEncoding & r);
std::ostream& operator<<(std::ostream& os, const http::Request& r);
std::ostream& operator<<(std::ostream&, const http::parameter&);
std::ostream& operator<<(std::ostream&, const http::transfer_extension&);
std::ostream& operator<<(std::ostream&, const http::url::URL&);
#endif
