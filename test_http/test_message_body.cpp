#include "../http.hpp"
#include <gtest/gtest.h>

TEST(TestMessageBody, CalculateLength1) {
  char message[] = "Transfer-Encoding:gzip, chunked\r\n"
	   "\r\n";
  char *ptr = message;
  http::message current;
  current.parse_headers(current.headers_, ptr);
  current.header_analysis();
  current.calculate_length_message();
  ASSERT_EQ(current.message_info_.length_, "chunked");
}

TEST(TestMessageBody, CalculateLength2) {
  char message[] = "Transfer-Encoding:gzip\r\n"
				   "\r\n";
  char *ptr = message;
  http::message current;
  current.parse_headers(current.headers_, ptr);
  current.header_analysis();
  ASSERT_ANY_THROW(current.calculate_length_message());
}

TEST(TestMessageBody, CalculateLength3) {
  char message[] = "Transfer-Encoding:gzip, chunked\r\n"
				   "Content-Length:42\r\n"
				   "\r\n";
  char *ptr = message;
  http::message current;
  current.parse_headers(current.headers_, ptr);
  current.header_analysis();
  ASSERT_ANY_THROW(current.calculate_length_message());
}

TEST(TestMessageBody, CalculateLength4) {
  char message[] = "Content-Length:42\r\n"
				   "\r\n";
  char *ptr = message;
  http::message current;
  current.parse_headers(current.headers_, ptr);
  current.header_analysis();
  current.calculate_length_message();
  ASSERT_EQ(current.message_info_.length_, "content-length");
}

TEST(TestMessageBody, CalculateLength5) {
  char message[] = "\r\n";
  char *ptr = message;
  http::message current;
  current.parse_headers(current.headers_, ptr);
  current.header_analysis();
  current.calculate_length_message();
  ASSERT_EQ(current.message_info_.length_, "empty");
}
