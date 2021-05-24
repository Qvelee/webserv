#include "../http.hpp"
#include <gtest/gtest.h>

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