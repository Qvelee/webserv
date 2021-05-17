#include "../http.hpp"
#include <gtest/gtest.h>


// transfer-encoding
TEST(TestParserTransferEncoding, Simple) {
  char message[] = "Transfer-Encoding:chunked\r\n"
				   "\r\n";
  std::vector<http::transfer_parameter> tp = {};
  http::TransferEncoding expected = {
	  {"chunked", tp},
  };
  http::Request req;
  http::parse_headers(req.headers, message);
  http::header_analysis(req, req.headers);
  ASSERT_EQ(req.transfer_encoding, expected);
}

// content-length
TEST(TestParserContentLength, ContentLength) {
  char message[] = "Content-Length:42\r\n"
				   "\r\n";
  http::Request req;
  http::parse_headers(req.headers, message);
  http::header_analysis(req, req.headers);
  ASSERT_EQ(42, req.content_length);
}

TEST(TestParserContentLength, TwoHeadersWithContentLength) {
  char message[] = "Content-Length:42\r\n"
				   "Content-Length:42\r\n"
				   "\r\n";
  http::Request req;
  http::parse_headers(req.headers, message);
  ASSERT_ANY_THROW(http::header_analysis(req, req.headers));
}

TEST(TestParserContentLength, TwoValueInContentLength) {
  char message[] = "Content-Length:42,42\r\n"
				   "\r\n";
  http::Request req;
  http::parse_headers(req.headers, message);
  ASSERT_ANY_THROW(http::header_analysis(req, req.headers));
}

TEST(TestParserContentLength, MoreValueInContentLength) {
  char message[] = "Content-Length:42, 42,,  ,   42  \r\n"
				   "\r\n";
  http::Request req;
  http::parse_headers(req.headers, message);
  ASSERT_ANY_THROW(http::header_analysis(req, req.headers));
}

TEST(TestParserContentLength, DifferentValueInContentLength) {
  char message[] = "Content-Length:42\r\n"
				   "Content-Length:43\r\n"
				   "\r\n";
  http::Request req;
  http::parse_headers(req.headers, message);
  ASSERT_ANY_THROW(http::header_analysis(req, req.headers));
}

TEST(TestParserContentLength, DifferentValueInOneHeader) {
  char message[] = "Content-Length:42, 43\r\n"
				   "\r\n";
  http::Request req;
  http::parse_headers(req.headers, message);
  ASSERT_ANY_THROW(http::header_analysis(req, req.headers));
}

TEST(TestParserContentLength, EmptyContentLength) {
  char message[] = "Content-Length:  , , \r\n"
				   "\r\n";
  http::Request req;
  http::parse_headers(req.headers, message);
  ASSERT_ANY_THROW(http::header_analysis(req, req.headers));
}

TEST(TestParserContentLength, BigNumber) {
  char message[] = "Content-Length:99999999999999999999999\r\n"
				   "\r\n";
  http::Request req;
  http::parse_headers(req.headers, message);
  ASSERT_ANY_THROW(http::header_analysis(req, req.headers));
}

TEST(TestMessageBody, CalculateLength1) {
  char message[] = "Transfer-Encoding:chunked\r\n"
				   "\r\n";
  http::Request req;
  http::parse_headers(req.headers, message);
  http::header_analysis(req, req.headers);
  http::calculate_length_message(req);
  ASSERT_EQ(req.content_length, -1);
}

TEST(TestMessageBody, CalculateLength2) {
  char message[] = "Transfer-Encoding:gzip\r\n"
				   "\r\n";
  http::Request req;
  http::parse_headers(req.headers, message);
  ASSERT_ANY_THROW(http::header_analysis(req, req.headers));
}

TEST(TestMessageBody, CalculateLength3) {
  char message[] = "Transfer-Encoding:chunked\r\n"
				   "Content-Length:42\r\n"
				   "\r\n";
  http::Request req;
  http::parse_headers(req.headers, message);
  http::header_analysis(req, req.headers);
  ASSERT_ANY_THROW(http::calculate_length_message(req));
}

TEST(TestMessageBody, CalculateLength4) {
  char message[] = "Content-Length:42\r\n"
				   "\r\n";
  http::Request req;
  http::parse_headers(req.headers, message);
  http::header_analysis(req, req.headers);
  http::calculate_length_message(req);
  ASSERT_EQ(req.content_length, 42);
}

TEST(TestMessageBody, CalculateLength5) {
  char message[] = "\r\n";
  http::Request req;
  http::parse_headers(req.headers, message);
  http::header_analysis(req, req.headers);
  http::calculate_length_message(req);
  ASSERT_EQ(req.content_length, 0);
}

TEST(TestReadBody, ContentLength) {
  char message[] = "Content-Length:14\r\n"
				   "\r\n";
  char body[] = "Hello, world!\n";
  http::Request req;
  http::parse_headers(req.headers, message);
  http::header_analysis(req, req.headers);
  http::calculate_length_message(req);
  http::read_body(req, body);
  ASSERT_EQ(body, req.body);
}

TEST(TestReadBody, ContentLength2) {
  char message[] = "Content-Length:13\r\n"
				   "\r\n";
  char body[] = "Hello, world!\n";
  char tmp[] = "Hello, world!";
  http::Request req;
  http::parse_headers(req.headers, message);
  http::header_analysis(req, req.headers);
  http::calculate_length_message(req);
  http::read_body(req, body);
  ASSERT_EQ(tmp, req.body);
}

TEST(TestReadBody, ContentLength3) {
  char message[] = "Content-Length:15\r\n"
				   "\r\n";
  char body[] = "Hello, world!\n";
  http::Request req;
  http::parse_headers(req.headers, message);
  http::header_analysis(req, req.headers);
  http::calculate_length_message(req);
  http::read_body(req, body);
  ASSERT_EQ(body, req.body);
}

TEST(TestReadBody, ContentLength4) {
  char message[] = "Content-Length:14\r\n"
				   "\r\n";
  char halfBody[] = "Hello,";
  char body[] = "Hello, world!\n";
  http::Request req;
  http::parse_headers(req.headers, message);
  http::header_analysis(req, req.headers);
  http::calculate_length_message(req);
  http::read_body(req, halfBody);
  ASSERT_EQ(halfBody, req.body);
  http::read_body(req, body + 6);
  ASSERT_EQ(body, req.body);
  http::read_body(req, halfBody);
  ASSERT_EQ(body, req.body);
}

TEST(ReadBody, Chunked) {
  char header[] = "Transfer-Encoding: chunked\r\n"
				  "\r\n";
  char message1[] = "D\r\n"
				   "Hello, World\n\r\n";
  char message2[] = "13\r\n"
				   "I'm chunked coding!\r\n";
  char message3[] = "0\r\n"
					"\r\n";
  http::Request req;
  http::parse_headers(req.headers, header);
  http::header_analysis(req, req.headers);
  http::calculate_length_message(req);
  http::read_body(req, message1);
  ASSERT_EQ("Hello, World\n", req.body);
  http::read_body(req, message2);
  ASSERT_EQ("Hello, World\nI'm chunked coding!", req.body);
  http::read_body(req, message3);
}

TEST(ReadBody, Chunked2) {
  char header[] = "Transfer-Encoding: chunked\r\n"
				  "\r\n";
  char message1[] = "D;name1=val1\r\n"
					"Hello, World\n\r\n";
  char message2[] = "13;name2=val2;name3;name4=\"sdfsdf\\\\fs\"\r\n"
					"I'm chunked coding!\r\n";
  char message3[] = "0\r\n"
					"\r\n";
  http::Request req;
  http::parse_headers(req.headers, header);
  http::header_analysis(req, req.headers);
  http::calculate_length_message(req);
  http::read_body(req, message1);
  ASSERT_EQ("Hello, World\n", req.body);
  http::read_body(req, message2);
  ASSERT_EQ("Hello, World\nI'm chunked coding!", req.body);
  http::read_body(req, message3);
}

TEST(ReadBody, Chunked3) {
  char header[] = "Transfer-Encoding: chunked\r\n"
				  "\r\n";
  char message1[] = "D;name1=val1\r\n"
					"Hello, World\n\r\n";
  char message2[] = "13;name2=val2;name3;name4=\"sdfsdf\\\\fs\"\r\n"
					"I'm chunked coding!\r\n";
  char message3[] = "0\r\n"
					"Field1:value1\r\n"
					"\r\n";
  http::Request req;
  http::parse_headers(req.headers, header);
  http::header_analysis(req, req.headers);
  http::calculate_length_message(req);
  http::read_body(req, message1);
  ASSERT_EQ("Hello, World\n", req.body);
  http::read_body(req, message2);
  ASSERT_EQ("Hello, World\nI'm chunked coding!", req.body);
  http::read_body(req, message3);
}