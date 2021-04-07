#include "http.hpp"
#include <cstring>
#include "utility_http.hpp"
#include "error_http.hpp"
#include <sstream>

namespace http {

const std::map<std::string, void (message::*)()>
    message::header_field_handlers = {
	{"content-length", &message::content_length},
	{"transfer-encoding", &message::transfer_encoding},
};

message::message(char *bytes) {
  parse_start_line(bytes);
  parse_headers(bytes);
  header_analysis();
//  parse_message_body(bytes);
}

void message::parse_start_line(char *&bytes) {
  char *begin_word = bytes;

  while (istchar(*bytes))
    ++bytes;
  if (bytes - begin_word > 15)
    error(501);
  if (*bytes != *SP)
    error(400);
  start_line_.method_.assign(begin_word, bytes - begin_word);

  ++bytes;
  begin_word = bytes;

  while (*bytes != *SP && *bytes != '\0')
    ++bytes;
  if (*bytes != *SP)
    error(400);
  if (bytes - begin_word > 8000)
    error(414);
  start_line_.request_target_.assign(begin_word, bytes - begin_word);

  ++bytes;
  begin_word = bytes;

  if (strncmp(bytes, "HTTP/", 5) != 0)
    error(400);
  bytes += 5;
  if (!isdigit(*bytes))
    error(400);
  ++bytes;
  if (*bytes != '.')
    error(400);
  ++bytes;
  if (!isdigit(*bytes))
    error(400);
  ++bytes;

  start_line_.http_version.assign(begin_word, bytes - begin_word);
  if (strncmp(bytes, "\r\n", 2) != 0)
    error(400);
  bytes += 2;
}

void message::parse_headers(char *&bytes) {
  char			*begin_word = bytes;
  std::string	field_name;
  std::string	field_value;

  if (*bytes == *SP)
    error(400);

  while (strncmp(CRLF, bytes, 2) != 0) {
    while (istchar(*bytes))
      ++bytes;
    if (*bytes != ':')
      error(400);
    field_name.assign(begin_word, bytes - begin_word);
    tolower(field_name);
    ++bytes;

	while (isblank(*bytes))
	  ++bytes;
	begin_word = bytes;

	do {
	  bytes = begin_word;
	  while (!(*bytes >= 0 && *bytes <= 127) || isgraph(*bytes) ||
	  isblank(*bytes))
	    ++bytes;
	  if (strncmp(CRLF, bytes, 2) != 0)
	    error(400);
	  field_value.append(begin_word, bytes - begin_word);
	  field_value.append(SP);
	  begin_word = bytes + 2;
	  while (isblank(*begin_word))
	    ++begin_word;
	} while (isblank(*(bytes + 2)));

	bytes = begin_word;
	while (isblank(*field_value.rbegin()))
	  field_value.erase(field_value.end() - 1);

	headers_[field_name].push_back(field_value);
	field_value.clear();
	field_name.clear();
  }
  bytes += 2;
}

void message::header_analysis() {
  std::map<std::string, std::vector<std::string> >::const_iterator first;
  std::map<std::string, std::vector<std::string> >::const_iterator last;
  first = headers_.begin();
  last = headers_.end();

  for (; first != last; ++first) {
    if (header_field_handlers.count(first->first) == 1)
	  (this->*header_field_handlers.at(first->first))();
  }
}

void message::content_length() {
  std::vector<std::string>& tmp = headers_["content-length"];
  std::size_t tmp_length;
  std::size_t first_length;
  char c;

  for (std::size_t i = 0; i < tmp.size(); ++i) {
	if (tmp[i].find_first_not_of("0123456789\t ,") != tmp[i].npos)
	  error(400);
	std::stringstream ss(tmp[i]);
	if (i == 0) {
	  ss >> first_length;
	  if (ss.fail())
	    error(400);
	  do {
		ss.get(c);
		if (ss.eof())
		  break ;
	  } while (c == ' ' || c == ',');
	  if (!ss.eof())
	    ss.putback(c);
	}
	while (!ss.eof()) {
	  ss >> tmp_length;
	  if (ss.fail())
	    error(400);
	  if (tmp_length != first_length)
	    error(400);
	  do {
	    ss.get(c);
	    if (ss.eof())
		  break ;
	  } while (c == ' ' || c == ',');
	  if (!ss.eof())
		ss.putback(c);
	}
//    if (ss.fail())
//      error(400);
  }
  message_info_.content_length = first_length;
}

void message::transfer_encoding() {

}

}