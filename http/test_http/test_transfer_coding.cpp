#include "../http.hpp"
#include <gtest/gtest.h>
#include "../test_http/utility.hpp"

TEST(TestParserTransferEncoding, TransferEncoding1) {
  std::string message = "Transfer-Encoding: chunked  ;  size=1\r\n"
						"\r\n";
  std::vector<http::parameter> tp = {
	  {"size", "1"},
  };
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

TEST(TestParserTransferEncoding, TransferEncoding3) {
  std::string message = "Transfer-Encoding:chunked  ;  size   =   1  \r\n"
						"\r\n";
  std::vector<http::parameter> tp = {
	  {"size", "1"},
  };
  http::TransferEncoding expected = {
	  {"chunked",tp},
  };
  http::Request req;
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  ASSERT_EQ(err, 0);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 0);
  ASSERT_EQ(req.transfer_encoding, expected);
}

TEST(TestParserTransferEncoding, TransferEncoding4) {
  std::string message = "Transfer-Encoding:chunked;size=1\r\n"
						"\r\n";
  std::vector<http::parameter> tp = {
	  {"size", "1"},
  };
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

TEST(TestParserTransferEncoding, TransferEncoding5) {
  std::string message = "Transfer-Encoding:chunked,  chunked,  , ,  \r\n"
						"\r\n";
  std::vector<http::parameter> tp = {};
  http::TransferEncoding expected = {
	  {"chunked", tp},
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

TEST(TestParserTransferEncoding, TransferEncoding6) {
  std::string message = "Transfer-Encoding:chunked ;size=45, chunked;   size  =  23\r\n"
						"\r\n";
  std::vector<http::parameter> tp1 = {
	  {"size", "45"},
  };
  std::vector<http::parameter> tp2 = {
	  {"size", "23"},
  };
  http::TransferEncoding expected = {
	  {"chunked", tp1},
	  {"chunked", tp2},
  };
  http::Request req;
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  ASSERT_EQ(err, 0);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 0);
  ASSERT_EQ(req.transfer_encoding, expected);
}

TEST(TestParserTransferEncoding, TransferEncoding7) {
  std::string message = "Transfer-Encoding:chunkEd ;size=45, chunked;   size  =  23\r\n"
						"\r\n";
  std::vector<http::parameter> tp1 = {
	  {"size", "45"},
  };
  std::vector<http::parameter> tp2 = {
	  {"size", "23"},
  };
  http::TransferEncoding expected = {
	  {"chunked", tp1},
	  {"chunked", tp2},
  };
  http::Request req;
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  ASSERT_EQ(err, 0);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 0);
  ASSERT_EQ(req.transfer_encoding, expected);
}

TEST(TestParserTransferEncoding, TransferEncoding8) {
  std::string message = "Transfer-Encoding:chunked ;size=\"45\", chunked;   size"
						"  =   \"2\\\"3\"\r\n"
						"\r\n";
  std::vector<http::parameter> tp1 = {
	  {"size", "45"},
  };
  std::vector<http::parameter> tp2 = {
	  {"size", "2\"3"},
  };
  http::TransferEncoding expected = {
	  {"chunked", tp1},
	  {"chunked", tp2},
  };
  http::Request req;
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  ASSERT_EQ(err, 0);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 0);
  ASSERT_EQ(req.transfer_encoding, expected);
}

TEST(TestParserTransferEncoding, TransferEncodingFail1) {
  std::string message = "Transfer-Encoding:gzip ;size=\"45, chunked;   size  =  23\r\n"
						"\r\n";
  http::Request req;
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  ASSERT_EQ(err, 0);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 501);
}

TEST(TestParserTransferEncoding, TransferEncodingFail2) {
  std::string message = "Transfer-Encoding:gzi ;size=45, chunked;   size  =  23\r\n"
						"\r\n";

  http::Request req;
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  ASSERT_EQ(err, 0);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 501);

}

TEST(TestParserTransferEncoding, TransferEncoding9) {
  std::string message = "Transfer-Encoding:chunked ;size=45 ;size=87, chunked;   size  =  "
						"23\r\n"
						"\r\n";
  std::vector<http::parameter> tp1 = {
	  {"size", "45"},
	  {"size", "87"}
  };
  std::vector<http::parameter> tp2 = {
	  {"size", "23"},
  };
  http::TransferEncoding expected = {
	  {"chunked", tp1},
	  {"chunked", tp2},
  };
  http::Request req;
  http::StatusCode err = http::NoError;
  http::parse_headers(req.headers, message, 0, err);
  ASSERT_EQ(err, 0);
  http::header_analysis(req, req.headers, err);
  ASSERT_EQ(err, 0);
  ASSERT_EQ(req.transfer_encoding, expected);
}
