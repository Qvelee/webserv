#ifndef WEBSERV_HTTP_HPP
#define WEBSERV_HTTP_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>

namespace http {

class message {
 public:
  message(const char *bytes);
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

  struct transfer_parameter {
    std::string name_;
    std::string value_;

    bool operator==(const http::message::transfer_parameter& rhs) const {
      if (name_ != rhs.name_)
		return false;
      if (value_ != rhs.value_)
		return false;
	  return true;
    }
  };

  struct transfer_extension {
    std::string token_;
    transfer_parameter transfer_parameter_;

    bool operator==(const http::message::transfer_extension& rhs) const {
      if (token_ != rhs.token_)
		return false;
      if (!(transfer_parameter_ == rhs.transfer_parameter_))
		return false;
	  return true;
    }
  };

  struct message_info {
    std::size_t content_length_;
    std::vector<transfer_extension> transfer_coding_;
	std::string length_;

	bool operator==(const http::message::message_info &rhs) const {
      if (content_length_ != rhs.content_length_)
		return false;
      if (transfer_coding_ != rhs.transfer_coding_)
		return false;
	  if (length_ != rhs.length_)
		return false;
	  return true;
    }
  };

  static const std::map<std::string, void (message::*)()> header_field_handlers;
  static std::size_t parse_request_line(request_line &rl, const char *bytes);
  static std::size_t parse_headers(std::map<std::string, std::string>& dst, const char *bytes);
  void header_analysis();
  void calculate_length_message();
  void read_message_body(const char *bytes);

  std::size_t read_chunk_size(const char *bytes);
  void decoding_chunked(const char *bytes);

  // header_field_handlers
  static const std::map<std::string, int> transfer_coding_registration;

  void content_length();
  static void validate_transfer_coding(const std::string& name);
  void transfer_encoding();

// private:
  message() {}

  request_line							start_line_;
  std::map<std::string, std::string>	headers_;
  std::string							decoded_body_;
  message_info							message_info_;
};
}

std::ostream& operator<<(std::ostream&, const http::message::request_line&);
std::ostream& operator<<(std::ostream&, const http::message::message_info&);
std::ostream& operator<<(std::ostream&, const http::message::transfer_parameter&);
std::ostream& operator<<(std::ostream&, const http::message::transfer_extension&);

#endif
