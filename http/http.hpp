#ifndef WEBSERV_HTTP_HPP
#define WEBSERV_HTTP_HPP

#include <string>
#include <utility>
#include <vector>
#include <map>
#include <iostream>
#include "url.hpp"
#include <inttypes.h>
#include "WebservConf.hpp"

namespace http {

enum Method {
  GET,
  POST,
  DELETE
};

enum StatusCode {
  NoError = 0,
//  StatusContinue = 100,
//  StatusSwitchingProtocols = 101,
  StatusOK = 200,
  StatusCreated = 201,
//  StatusAccepted = 202,
//  StatusNonAuthoritativeInfo = 203,
//  StatusNoContent = 204,
//  StatusResetContent = 205,
//  StatusMultipleChoices = 300,
  StatusMovedPermanently = 301,
  StatusFound = 302,
  StatusSeeOther = 303,
  //  StatusUseProxy = 305,
  StatusTemporaryRedirect = 307,
  StatusBadRequest = 400,
//  StatusPaymentRequired = 402,
  StatusForbidden = 403,
  StatusNotFound = 404,
  StatusMethodNotAllowed = 405,
//  StatusNotAcceptable = 406,
  StatusRequestTimeout = 408,
//  StatusConflict = 409,
//  StatusGone = 410,
//  StatusLengthRequired = 411,
  StatusRequestEntityTooLarge = 413,
  StatusRequestURITooLong = 414,
//  StatusUnsupportedMediaType = 415,
//  StatusExpectationFailed = 417,
//  StatusUpgradeRequired = 426,
  StatusInternalServerError = 500,
  StatusNotImplemented = 501,
//  StatusBadGateway = 502,
  StatusServiceUnavailable = 503,
//  StatusGatewayTimeout = 504,
  StatusHTTPVersionNotSupported = 505
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
  media_type media_type_;
  std::vector<std::string> content_language;
  url::URL content_location;
};

typedef std::map<std::string, std::string> Headers;
typedef std::vector<transfer_extension> TransferEncoding;

struct Request {
  Method method;
  url::URL url;
  std::string proto;
  Headers headers;
  int64_t content_length;
  TransferEncoding transfer_encoding;
  bool close;
  Headers trailer;
  std::string body;
  representation_metadata metadata;
  StatusCode code;
  config::tServerInformation serv_config;
};

typedef void (*field_handlers)(Request &req, std::string const &field, StatusCode &code);
typedef std::map<std::string, field_handlers> HeaderHandlers;
typedef std::map<std::string, int> TransferCodingRegister;
typedef std::map<std::string, int> MediaTypeRegister;

bool parse_request(Request &req,
				   std::string const &data,
				   std::map<std::string, config::tServer> const &conf);
size_t parse_request_line(Request &r,
						  std::string const &data,
						  size_t begin,
						  StatusCode &code);
void parse_and_validate_method(Method &m, std::string const &src, StatusCode &code);
void parse_request_target(url::URL &url,
						  Method &m,
						  std::string const &src,
						  StatusCode &code);
size_t parse_headers(Headers &dst,
					 std::string const &data,
					 size_t begin,
					 StatusCode &code);
void header_analysis(Request &r, Headers &h, StatusCode &code);
void calculate_length_message(Request &req, StatusCode &code);
bool read_body(Request &req, std::string const &data, size_t begin, StatusCode &code);
bool add_body(Request &req, std::string const &date);
bool decoding_chunked(Request &req,
					  std::string const &data,
					  size_t begin,
					  StatusCode &code);
size_t read_chunk_size(size_t &size,
					   std::string const &data,
					   size_t begin,
					   StatusCode &code);
bool check_config(Request &req);

//header_field_handlers
HeaderHandlers init_header_field_handlers();
TransferCodingRegister init_transfer_coding_register();
MediaTypeRegister init_media_type_register();

void transfer_encoding(Request &req, std::string const &value, StatusCode &code);
void validate_transfer_coding(const std::string &name, StatusCode &code);
void content_length(Request &req, std::string const &value, StatusCode &code);
void host(Request &req, std::string const &value, StatusCode &code);
void connection(Request &req, std::string const &value, StatusCode &code);
void content_type(Request &req, std::string const &value, StatusCode &code);
void validate_media_type(const media_type &type, StatusCode &code);
void content_language(Request &req, std::string const &value, StatusCode &code);
void content_location(Request &req, std::string const &value, StatusCode &code);

struct Response {
  std::string status;
  StatusCode code;
  Headers header;
  std::string body;

  Response();
};

void method_get(const Request &req, Response &resp);
void method_post(const Request &req, Response &response);
void method_delete(const Request &req, Response &response);
void get_response(const Request &req, Response &resp);
void ResponseToString(const Response &resp, std::string &str);

}

#endif
