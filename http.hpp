#ifndef WEBSERV_HTTP_HPP
#define WEBSERV_HTTP_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>

namespace http {

const char* const CR = "\r";
const char* const LF = "\n";
const char* const HTAB = "\t";
const char* const DQUOTE = "\"";
const char* const SP = " ";
const char* const CRLF = "\r\n";
//CTL = isctrl();
//DIGIT = isdigit();
//HEXDIG = isxdigit();
//VCHAR = isgraph();
//WSP = isblank();
//OCTET = isprint() || iscntrl();
//LWSP = *(WSP / CRLF WSP);
//OWS = *(SP / HTAB)

class message {
 public:
  message(char *bytes);
// protected:
  struct request_line {
    std::string method_;
    std::string request_target_;
    std::string http_version;

    bool operator==(const http::message::request_line &rhs) const {
      if (method_ != rhs.method_)
        return false;
      if (request_target_ != rhs.request_target_)
        return false;
      if (http_version != rhs.http_version)
        return false;
      return true;
    }
  };

  struct status_line {

  };

  struct message_info {
    std::size_t content_length;

    bool operator==(const http::message::message_info &rhs) const {
      if (content_length != rhs.content_length)
		return false;
	  return true;
    }
  };

  static const std::map<std::string, void (message::*)()> header_field_handlers;
  void parse_start_line(char *&bytes);
  void parse_headers(char *&bytes);
  void header_analysis();
  void parse_message_body(char *&bytes);

  // header_field_handlers
  void content_length();
  void transfer_encoding();

// private:
  message();

  request_line										start_line_;
  std::map<std::string, std::vector<std::string> >	headers_;
  char												*message_body_;
  message_info										message_info_;
};
}

std::ostream& operator<<(std::ostream&, const http::message::request_line&);
std::ostream& operator<<(std::ostream&, const http::message::message_info&);

#endif
