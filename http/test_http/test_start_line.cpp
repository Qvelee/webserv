#include "../http.hpp"
#include "../test_http/utility.hpp"
#include <gtest/gtest.h>

TEST(TestParserStartLine, NoFail) {
  std::string message = "GET /me HTTP/1.1\r\n";
  http::Request expected = {
	  .method = http::GET,
	  .url = {
		  .path = "/me",
	  },
	  .proto = "HTTP/1.1",
	  .content_length = 0,
  };
  http::StatusCode err = http::NoError;
  http::Request current = {
	  .content_length = 0,
  };
  http::parse_request_line(current, message, 0, err);
  ASSERT_EQ(current, expected);
  ASSERT_EQ(err, 0);
}

TEST(TestParserStartLine, EmptyMethod) {
  std::string message = " /me HTTP/1.1\r\n";
  http::Request current;
  http::StatusCode err = http::NoError;
  http::parse_request_line(current, message, 0, err);
  ASSERT_EQ(err, 400);
}

TEST(TestParserStartLine, OddSpace1) {
  std::string message = "GET  /me HTTP/1.1\r\n";
  http::Request current;
  http::StatusCode err = http::NoError;
  http::parse_request_line(current, message, 0, err);
  ASSERT_EQ(err, 400);
}

TEST(TestParserStartLine, OddSpace2) {
  std::string message = "GET /me HTTP/1.1 \r\n";
  http::Request current;
  http::StatusCode err = http::NoError;
  http::parse_request_line(current, message, 0, err);
  ASSERT_EQ(err, 400);
}

TEST(TestParserStartLine, OddSpace3) {
  std::string message = "GET /me  HTTP/1.1\r\n";
  http::Request current;
  http::StatusCode err = http::NoError;
  http::parse_request_line(current, message, 0, err);
  ASSERT_EQ(err, 505);
}

TEST(TestParserStartLine, WrongProtocol) {
  std::string message = "GET /me HTTP/1.10\r\n";
  http::Request current;
  http::StatusCode err = http::NoError;
  http::parse_request_line(current, message, 0, err);
  ASSERT_EQ(err, 400);
}

TEST(TestParserStartLine, NoCRLF) {
  std::string message = "GET /me HTTP/1.1";
  http::Request current;
  http::StatusCode err = http::NoError;
  http::parse_request_line(current, message, 0, err);
  ASSERT_EQ(err, 400);
}

TEST(TestParserStartLine, CaseSensetive) {
  std::string message = "GET /me HTtP/1.1\r\n";
  http::Request current;
  http::StatusCode err = http::NoError;
  http::parse_request_line(current, message, 0, err);
  ASSERT_EQ(err, 505);
}

TEST(TestParserStartLine, WrongProtocol2) {
  std::string message = "GET /me HTTP/1.2\r\n";
  http::Request current;
  http::StatusCode err = http::NoError;
  http::parse_request_line(current, message, 0, err);
  ASSERT_EQ(err, 505);
}

TEST(TestParserStartLine, url) {
  std::string message = "GET /me?param1=one&param2=two HTTP/1.1\r\n";
  http::Request current;
  http::Request expected = {
	  .method = http::GET,
	  .url= {
		  .path = "/me",
		  .raw_query = "param1=one&param2=two",
	  },
	  .proto= "HTTP/1.1",
  };
  http::StatusCode err = http::NoError;
  http::parse_request_line(current, message, 0, err);
  ASSERT_EQ(current, expected);
  ASSERT_EQ(err, 0);
}

//TEST(TestParserStartLine, url2) {
//  std::string message = "CONNECT www.example.com:80 HTTP/1.1\r\n";
//  http::Request current;
//  http::Request expected = {
//	  .method = http::CONNECT,
//	  .url = {
//		  .host = "www.example.com:80"
//	  },
//	  .proto = "HTTP/1.1",
//  };
//  http::StatusCode err = http::NoError;
//  http::parse_request_line(current, message, 0, err);
//  ASSERT_EQ(current, expected);
//  ASSERT_EQ(err, 0);
//}

