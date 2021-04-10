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

TEST(TestParserStartLine, SpaceBeforeMethod) {
  char message[] = " GET /me HTTP/1.1\r\n\r\n";
  char *ptr = message;
  http::message current;
  ASSERT_ANY_THROW(current.parse_request_line(current.start_line_, ptr));
}

TEST(TestParserStartLine, NotTcharInMethod) {
  char message[] = "G(ET /me HTTP/1.1\r\n\r\n";
  char *ptr = message;
  http::message current;
  ASSERT_ANY_THROW(current.parse_request_line(current.start_line_, ptr));
}

TEST(TestParserStartLine, OddSpace1) {
  char message[] = "GET  /me HTTP/1.1\r\n\r\n";
  char *ptr = message;
  http::message current;
  ASSERT_ANY_THROW(current.parse_request_line(current.start_line_, ptr));
}

TEST(TestParserStartLine, OddSpace2) {
  char message[] = "GET /me HTTP/1.1 \r\n\r\n";
  char *ptr = message;
  http::message current;
  ASSERT_ANY_THROW(current.parse_request_line(current.start_line_, ptr));
}

TEST(TestParserStartLine, OddSpace3) {
  char message[] = "GET /me  HTTP/1.1\r\n\r\n";
  char *ptr = message;
  http::message current;
  ASSERT_ANY_THROW(current.parse_request_line(current.start_line_, ptr));
}

TEST(TestParserStartLine, WrongProtoco1) {
  char message[] = "GET /me HTTp/1.1\r\n\r\n";
  char *ptr = message;
  http::message current;
  ASSERT_ANY_THROW(current.parse_request_line(current.start_line_, ptr));
}

TEST(TestParserStartLine, WrongProtoco2) {
  char message[] = "GET /me HTTP/10.1\r\n\r\n";
  char *ptr = message;
  http::message current;
  ASSERT_ANY_THROW(current.parse_request_line(current.start_line_, ptr));
}

TEST(TestParserStartLine, WrongProtoco3) {
  char message[] = "GET /me HTTP/1,1\r\n\r\n";
  char *ptr = message;
  http::message current;
  ASSERT_ANY_THROW(current.parse_request_line(current.start_line_, ptr));
}
