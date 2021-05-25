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
};

struct transfer_extension {
  std::string token;
  std::vector<parameter> transfer_parameter;
};

struct media_type {
  std::string type;
  std::string subtype;
  std::vector<parameter> parameters;
};

struct representation_metadata {
  media_type				media_type;
  std::vector<std::string>	content_language;
  url::URL					content_location;
};

typedef std::map<std::string, std::string>		Headers;
typedef std::vector<transfer_extension>			TransferEncoding;

struct Request {
  Method					method;
  url::URL					url;
  std::string				proto;
  Headers					headers;
  int64_t					content_length;
  TransferEncoding			transfer_encoding;
  Headers					trailer;
  std::string				body;
  representation_metadata	metadata;
  int						err;
};

typedef void (*field_handlers)(Request& req, std::string const &field, int &err);
typedef const std::map<std::string, field_handlers>	HeaderHandlers;
typedef const std::map<std::string, int>			TransferCodingRegister;
typedef const std::map<std::string, int>			MediaTypeRegister;

bool	parse_request(Request& req, std::string const &data);
size_t	parse_request_line(Request &r, std::string const &data, size_t begin, int &err);
void	parse_and_validate_method(Method &m, std::string const &src, int &err);
void	parse_request_target(url::URL &url, Method &m, std::string const &src, int &err);
size_t	parse_headers(Headers& dst, std::string const& data, size_t begin, int &err);
void	header_analysis(Request& r, Headers &h, int &err);
void	calculate_length_message(Request& req, int &err);
bool	read_body(Request &req, std::string const &data, size_t begin, int &err);
bool	decoding_chunked(Request& req, std::string const &data, size_t begin, int &err);
size_t	read_chunk_size(size_t& size, std::string const &data, size_t begin, int &err);

//header_field_handlers
HeaderHandlers&			get_header_field_handlers();
TransferCodingRegister&	get_transfer_coding_register();
MediaTypeRegister&		get_media_type_register();
void	transfer_encoding(Request& req, std::string const &value, int &err);
void	validate_transfer_coding(const std::string& name, int &err);
void	content_length(Request& req, std::string const &value, int &err);
void	host(Request& req, std::string const &value, int &err);
void 	content_type(Request &req, std::string const &value, int &err);
void	validate_media_type(const media_type& type, int &err);
void	content_language(Request &req, std::string const &value, int &err);
void	content_location(Request &req, std::string const &value, int &err);

//struct Response {
//};
}

#endif
