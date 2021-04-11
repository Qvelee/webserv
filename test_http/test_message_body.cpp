#include "../http.hpp"
#include <gtest/gtest.h>

TEST(TestMessageBody, CalculateLength1) {
  char message[] = "Transfer-Encoding:gzip, chunked\r\n"
	   "\r\n";
  char *ptr = message;
  http::message current;
  http::message::parse_headers(current.headers_, ptr);
  current.header_analysis();
  current.calculate_length_message();
  ASSERT_EQ(current.message_info_.length_, "chunked");
}

TEST(TestMessageBody, CalculateLength2) {
  char message[] = "Transfer-Encoding:gzip\r\n"
				   "\r\n";
  char *ptr = message;
  http::message current;
  http::message::parse_headers(current.headers_, ptr);
  current.header_analysis();
  ASSERT_ANY_THROW(current.calculate_length_message());
}

TEST(TestMessageBody, CalculateLength3) {
  char message[] = "Transfer-Encoding:gzip, chunked\r\n"
				   "Content-Length:42\r\n"
				   "\r\n";
  char *ptr = message;
  http::message current;
  http::message::parse_headers(current.headers_, ptr);
  current.header_analysis();
  ASSERT_ANY_THROW(current.calculate_length_message());
}

TEST(TestMessageBody, CalculateLength4) {
  char message[] = "Content-Length:42\r\n"
				   "\r\n";
  char *ptr = message;
  http::message current;
  http::message::parse_headers(current.headers_, ptr);
  current.header_analysis();
  current.calculate_length_message();
  ASSERT_EQ(current.message_info_.length_, "content-length");
}

TEST(TestMessageBody, CalculateLength5) {
  char message[] = "\r\n";
  char *ptr = message;
  http::message current;
  http::message::parse_headers(current.headers_, ptr);
  current.header_analysis();
  current.calculate_length_message();
  ASSERT_EQ(current.message_info_.length_, "empty");
}

TEST(ReadBody, Chunked) {
  char message[] = "D\r\n"
				   "Hello, World\n\r\n"
	   				"13\r\n"
					"I'm chunked coding!\r\n"
	 				"0\r\n"
	  				"\r\n";

  http::message current;
  current.decoding_chunked(message);
  ASSERT_EQ(current.size_decoded_body_, 32);
  ASSERT_EQ(current.decoded_body_, "Hello, World\n"
								   "I'm chunked coding!");
}

TEST(ReadBody, Chunked2) {
  char message[] = "D;name1=val1\r\n"
				   "Hello, World\n\r\n"
	   "13;name2=val2;name3;name4=\"sdfsdf\\\\fs\"\r\n"
				   "I'm chunked coding!\r\n"
				   "0\r\n"
				   "\r\n";

  http::message current;
  current.decoding_chunked(message);
  ASSERT_EQ(current.size_decoded_body_, 32);
  ASSERT_EQ(current.decoded_body_, "Hello, World\n"
								   "I'm chunked coding!");
}

TEST(ReadBody, Chunked3) {
  char message[] = "D;name1=val1\r\n"
				   "Hello, World\n\r\n"
				   "13;name2=val2;name3;name4=\"sdfsdf\\\\fs\"\r\n"
				   "I'm chunked coding!\r\n"
				   "0\r\n"
	   			   "Field1:value1\r\n"
				   "\r\n";

  http::message current;
  current.decoding_chunked(message);
  ASSERT_EQ(current.size_decoded_body_, 32);
  ASSERT_EQ(current.decoded_body_, "Hello, World\n"
								   "I'm chunked coding!");
}
