#include "../http.hpp"
#include <gtest/gtest.h>

std::ostream& operator<<(std::ostream& os, const http::message::request_line&
rl) {
  os << rl.method_ << ", ";
  os << rl.request_target_ << ", ";
  os << rl.http_version << "";
  return os;
}

TEST(TestParserStartLine, NoFail) {
  char message[] = "GET /me HTTP/1.1\r\n";
  http::message::request_line expected = {"GET", "/me","HTTP/1.1"};
  http::message::request_line current;
  http::message::parse_request_line(current, message);
  ASSERT_EQ(current, expected);
}

TEST(TestParserStartLine, EmptyMethod) {
  char message[] = " /me HTTP/1.1\r\n";
  http::message::request_line current;
  ASSERT_ANY_THROW(http::message::parse_request_line(current, message));
}

TEST(TestParserStartLine, OddSpace1) {
  char message[] = "GET  /me HTTP/1.1\r\n";
  http::message::request_line current;
  ASSERT_ANY_THROW(http::message::parse_request_line(current, message));
}

TEST(TestParserStartLine, OddSpace2) {
  char message[] = "GET /me HTTP/1.1 \r\n";
  http::message::request_line current;
  ASSERT_ANY_THROW(http::message::parse_request_line(current, message));
}

TEST(TestParserStartLine, OddSpace3) {
  char message[] = "GET /me  HTTP/1.1\r\n";
  http::message::request_line current;
  ASSERT_ANY_THROW(http::message::parse_request_line(current, message));
}

TEST(TestParserStartLine, WrongProtocol) {
  char message[] = "GET /me HTTP/1.10\r\n";
  http::message::request_line current;
  ASSERT_ANY_THROW(http::message::parse_request_line(current, message));
}

TEST(TestParserStartLine, NoCRLF) {
  char message[] = "GET /me HTTP/1.1";
  http::message::request_line current;
  ASSERT_ANY_THROW(http::message::parse_request_line(current, message));
}

/*
 * ADD validate test, big string test
 */