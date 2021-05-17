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

std::ostream& operator<<(std::ostream& os, const http::URL& url) {
  os << url.userinfo << url.host << url.raw_path << url.raw_query;
  return os;
}

TEST(TestParserStartLine, NoFail) {
  std::string message = "GET /me HTTP/1.1\r\n";
  http::Request expected = {
    http::GET,
	{
		.path = "/me",
      },
	"HTTP/1.1",
	};
  http::Request current;
  http::parse_request_line(current, message, 0);
  ASSERT_EQ(current, expected);
}

TEST(TestParserStartLine, EmptyMethod) {
  std::string message = " /me HTTP/1.1\r\n";
  http::Request current;
  ASSERT_ANY_THROW(http::parse_request_line(current, message, 0));
}

TEST(TestParserStartLine, OddSpace1) {
  std::string message = "GET  /me HTTP/1.1\r\n";
  http::Request current;
  ASSERT_ANY_THROW(http::parse_request_line(current, message, 0));
}

TEST(TestParserStartLine, OddSpace2) {
  std::string message = "GET /me HTTP/1.1 \r\n";
  http::Request current;
  ASSERT_ANY_THROW(http::parse_request_line(current, message, 0));
}

TEST(TestParserStartLine, OddSpace3) {
  std::string message = "GET /me  HTTP/1.1\r\n";
  http::Request current;
  ASSERT_ANY_THROW(http::parse_request_line(current, message, 0));
}

TEST(TestParserStartLine, WrongProtocol) {
  std::string message = "GET /me HTTP/1.10\r\n";
  http::Request current;
  ASSERT_ANY_THROW(http::parse_request_line(current, message, 0));
}

TEST(TestParserStartLine, NoCRLF) {
  std::string message = "GET /me HTTP/1.1";
  http::Request current;
  ASSERT_ANY_THROW(http::parse_request_line(current, message, 0));
}

TEST(TestParserStartLine, CaseSensetive) {
  std::string message = "GET /me HTtP/1.1\r\n";
  http::Request current;
  ASSERT_ANY_THROW(http::parse_request_line(current, message, 0));
}

TEST(TestParserStartLine, WrongProtocol2) {
  std::string message = "GET /me HTTP/1.2\r\n";
  http::Request current;
  ASSERT_ANY_THROW(http::parse_request_line(current, message, 0));
}

TEST(TestParserStartLine, url) {
  std::string message = "GET /me?param1=one&param2=two HTTP/1.1\r\n";
  http::Request current;
  http::Request expected = {
  	http::GET,
  	{
  		.path = "/me",
  		.raw_query = "param1=one&param2=two",
  	},
  	"HTTP/1.1",
  };
  http::parse_request_line(current, message, 0);
  ASSERT_EQ(current, expected);
}

TEST(TestParserStartLine, url2) {
  std::string message = "CONNECT www.example.com:80 HTTP/1.1\r\n";
  http::Request current;
  http::Request expected = {
	  http::CONNECT,
	  {
		  .host = "www.example.com:80"
	  },
	  "HTTP/1.1",
  };
  http::parse_request_line(current, message, 0);
  ASSERT_EQ(current, expected);
}
/*
 * ADD validate test, big string test
 */