#include "../http.hpp"
#include "../test_http/utility.hpp"
#include <gtest/gtest.h>

// transfer-encoding
TEST(TestParserTransferEncoding, Simple) {
  std::string message = "Transfer-Encoding:chunked\r\n"
						"\r\n";
  std::vector<http::parameter> tp = {};
  http::TransferEncoding expected = {
	  {"chunked", tp},
  };
  http::Request req;
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  ASSERT_EQ(err, 0);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 0);
  ASSERT_EQ(req.transfer_encoding, expected);
}

// content-length
TEST(TestParserContentLength, ContentLength) {
  std::string message = "Content-Length:42\r\n"
						"\r\n";
  http::Request req;
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  ASSERT_EQ(err, 0);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 0);
  ASSERT_EQ(42, req.content_length);
}

TEST(TestParserContentLength, TwoHeadersWithContentLength) {
  std::string message = "Content-Length:42\r\n"
						"Content-Length:42\r\n"
						"\r\n";
  http::Request req;
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 400);
}

TEST(TestParserContentLength, TwoValueInContentLength) {
  std::string message = "Content-Length:42,42\r\n"
						"\r\n";
  http::Request req;
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 400);
}

TEST(TestParserContentLength, MoreValueInContentLength) {
  std::string message = "Content-Length:42, 42,,  ,   42  \r\n"
						"\r\n";
  http::Request req;
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 400);
}

TEST(TestParserContentLength, DifferentValueInContentLength) {
  std::string message = "Content-Length:42\r\n"
						"Content-Length:43\r\n"
						"\r\n";
  http::Request req;
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 400);
}

TEST(TestParserContentLength, DifferentValueInOneHeader) {
  std::string message = "Content-Length:42, 43\r\n"
						"\r\n";
  http::Request req;
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 400);
}

TEST(TestParserContentLength, EmptyContentLength) {
  std::string message = "Content-Length:  , , \r\n"
						"\r\n";
  http::Request req;
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 400);
}

TEST(TestParserContentLength, BigNumber) {
  std::string message = "Content-Length:99999999999999999999999\r\n"
						"\r\n";
  http::Request req;
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 400);
}

TEST(TestMessageBody, CalculateLength1) {
  std::string message = "Transfer-Encoding:chunked\r\n"
						"\r\n";
  http::Request req = {
	  .content_length = -1,
  };
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  ASSERT_EQ(err, 0);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 0);
  http::calculate_length_message(req, err);
  ASSERT_EQ(err, 0);
  ASSERT_EQ(req.content_length, -1);
}

TEST(TestMessageBody, CalculateLength2) {
  std::string message = "Transfer-Encoding:gzip\r\n"
						"\r\n";
  http::Request req;
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 501);
}

TEST(TestMessageBody, CalculateLength3) {
  std::string message = "Transfer-Encoding:chunked\r\n"
						"Content-Length:42\r\n"
						"\r\n";
  http::Request req;
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  ASSERT_EQ(err, 0);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 0);
  http::calculate_length_message(req, err);
  ASSERT_EQ(err, 400);
}

TEST(TestMessageBody, CalculateLength4) {
  std::string message = "Content-Length:42\r\n"
						"\r\n";
  http::Request req;
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  ASSERT_EQ(err, 0);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 0);
  http::calculate_length_message(req, err);
  ASSERT_EQ(err, 0);
  ASSERT_EQ(req.content_length, 42);
}

TEST(TestMessageBody, CalculateLength5) {
  std::string message = "\r\n";
  http::Request req;
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  ASSERT_EQ(err, 0);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 0);
  http::calculate_length_message(req, err);
  ASSERT_EQ(err, 0);
  ASSERT_EQ(req.content_length, 0);
}

TEST(TestReadBody, ContentLength) {
  std::string message = "Content-Length:14\r\n"
						"\r\n";
  std::string body = "Hello, world!\n";
  http::Request req;
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  ASSERT_EQ(err, 0);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 0);
  http::calculate_length_message(req, err);
  ASSERT_EQ(err, 0);
  http::read_body(req, body, 0, err);
  ASSERT_EQ(err, 0);
  ASSERT_EQ(body, req.body);
}

TEST(TestReadBody, ContentLength2) {
  std::string message = "Content-Length:13\r\n"
						"\r\n";
  std::string body = "Hello, world!\n";
  char tmp[] = "Hello, world!";
  http::Request req;
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  ASSERT_EQ(err, 0);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 0);
  http::calculate_length_message(req, err);
  ASSERT_EQ(err, 0);
  http::read_body(req, body, 0, err);
  ASSERT_EQ(err, 0);
  ASSERT_EQ(tmp, req.body);
}

TEST(TestReadBody, ContentLength3) {
  std::string message = "Content-Length:15\r\n"
						"\r\n";
  std::string body = "Hello, world!\n";
  http::Request req;
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  ASSERT_EQ(err, 0);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 0);
  http::calculate_length_message(req, err);
  ASSERT_EQ(err, 0);
  http::read_body(req, body, 0, err);
  ASSERT_EQ(err, 0);
  ASSERT_EQ(body, req.body);
}

TEST(TestReadBody, ContentLength4) {
  std::string message = "Content-Length:14\r\n"
						"\r\n";
  std::string halfBody = "Hello,";
  std::string body = "Hello, world!\n";
  http::Request req;
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  ASSERT_EQ(err, 0);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 0);
  http::calculate_length_message(req, err);
  ASSERT_EQ(err, 0);
  http::read_body(req, halfBody, 0, err);
  ASSERT_EQ(err, 0);
  ASSERT_EQ(halfBody, req.body);
  http::read_body(req, body, 6, err);
  ASSERT_EQ(err, 0);
  ASSERT_EQ(body, req.body);
  http::read_body(req, halfBody, 0, err);
  ASSERT_EQ(err, 0);
  ASSERT_EQ(body, req.body);
}

TEST(ReadBody, Chunked) {
  std::string header = "Transfer-Encoding: chunked\r\n"
					   "\r\n";
  std::string message1 = "D\r\n"
						 "Hello, World\n\r\n";
  std::string message2 = "13\r\n"
						 "I'm chunked coding!\r\n";
  std::string message3 = "0\r\n"
						 "\r\n";
  http::Request req = {
	  .content_length = -1,
  };
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, header, 0, err);
  ASSERT_EQ(err, 0);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 0);
  http::calculate_length_message(req, err);
  ASSERT_EQ(err, 0);
  http::read_body(req, message1, 0, err);
  ASSERT_EQ(err, 0);
  ASSERT_EQ("Hello, World\n", req.body);
  http::read_body(req, message2, 0, err);
  ASSERT_EQ(err, 0);
  ASSERT_EQ("Hello, World\nI'm chunked coding!", req.body);
  http::read_body(req, message3, 0, err);
  ASSERT_EQ(err, 0);
}

TEST(ReadBody, Chunked2) {
  std::string header = "Transfer-Encoding: chunked\r\n"
					   "\r\n";
  std::string message1 = "D;name1=val1\r\n"
						 "Hello, World\n\r\n";
  std::string message2 = "13;name2=val2;name3;name4=\"sdfsdf\\\\fs\"\r\n"
						 "I'm chunked coding!\r\n";
  std::string message3 = "0\r\n"
						 "\r\n";
  http::Request req = {
	  .content_length = -1,
  };
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, header, 0, err);
  ASSERT_EQ(err, 0);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 0);
  http::calculate_length_message(req, err);
  ASSERT_EQ(err, 0);
  http::read_body(req, message1, 0, err);
  ASSERT_EQ(err, 0);
  ASSERT_EQ("Hello, World\n", req.body);
  http::read_body(req, message2, 0, err);
  ASSERT_EQ(err, 0);
  ASSERT_EQ("Hello, World\nI'm chunked coding!", req.body);
  http::read_body(req, message3, 0, err);
  ASSERT_EQ(err, 0);
}

TEST(ReadBody, Chunked3) {
  std::string header = "Transfer-Encoding: chunked\r\n"
					   "\r\n";
  std::string message1 = "D;name1=val1\r\n"
						 "Hello, World\n\r\n";
  std::string message2 = "13;name2=val2;name3;name4=\"sdfsdf\\\\fs\"\r\n"
						 "I'm chunked coding!\r\n";
  std::string message3 = "0\r\n"
						 "Field1:value1\r\n"
						 "\r\n";
  http::Request req = {
	  .content_length = -1,
  };
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, header, 0, err);
  ASSERT_EQ(err, 0);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 0);
  http::calculate_length_message(req, err);
  ASSERT_EQ(err, 0);
  ASSERT_EQ(http::read_body(req, message1, 0, err), false);
  ASSERT_EQ(err, 0);
  ASSERT_EQ("Hello, World\n", req.body);
  ASSERT_EQ(http::read_body(req, message2, 0, err), false);
  ASSERT_EQ(err, 0);
  ASSERT_EQ("Hello, World\nI'm chunked coding!", req.body);
  ASSERT_EQ(http::read_body(req, message3, 0, err), true);
  ASSERT_EQ(err, 0);
}

TEST(ReadBody, Chunked4) {
  std::string header = "Transfer-Encoding: chunked\r\n"
					   "\r\n";
  std::string message1 = "D;name1=val1\r\n"
						 "Hello, World\n\r\n";
  std::string message2 = "13;name2=val2;name3;name4=\"sdfsdf\\\\fs\"\r\n"
						 "I'm chunked coding!\r\n";
  std::string message3 = "0\r\n"
						 "Field1:value1\r\n"
						 "\r\n";
  http::Request req = {
	  .content_length = -1,
  };
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, header, 0, err);
  ASSERT_EQ(err, 0);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 0);
  http::calculate_length_message(req, err);
  ASSERT_EQ(err, 0);
  ASSERT_EQ(http::read_body(req, message1, 0, err), false);
  ASSERT_EQ(err, 0);
  ASSERT_EQ("Hello, World\n", req.body);
  ASSERT_EQ(http::add_body(req, message2), false);
  ASSERT_EQ(err, 0);
  ASSERT_EQ("Hello, World\nI'm chunked coding!", req.body);
  ASSERT_EQ(http::add_body(req, message3), true);
  ASSERT_EQ(err, 0);
}
