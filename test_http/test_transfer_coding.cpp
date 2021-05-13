#include "../http.hpp"
#include <gtest/gtest.h>



TEST(TestParserTransferEncoding, TransferEncoding1) {
  char message[] = "Transfer-Encoding: chunked  ;  size=1\r\n"
				 "\r\n";
  std::vector<http::transfer_parameter> tp = {
	  {"size", "1"},
  };
  http::TransferEncoding expected = {
  	{"chunked", tp},
  	};
  http::Request req;
  http::parse_headers(req.headers, message);
  http::header_analysis(req, req.headers);
  ASSERT_EQ(req.transfer_encoding, expected);
}

TEST(TestParserTransferEncoding, TransferEncoding3) {
  char message[] = "Transfer-Encoding:chunked  ;  size   =   1  \r\n"
				 "\r\n";
  std::vector<http::transfer_parameter> tp = {
	  {"size", "1"},
  };
  http::TransferEncoding expected = {
	  {"chunked",tp},
  };
  http::Request req;
  http::parse_headers(req.headers, message);
  http::header_analysis(req, req.headers);
  ASSERT_EQ(req.transfer_encoding, expected);
}

TEST(TestParserTransferEncoding, TransferEncoding4) {
  char message[] = "Transfer-Encoding:chunked;size=1\r\n"
				   "\r\n";
  std::vector<http::transfer_parameter> tp = {
	  {"size", "1"},
  };
  http::TransferEncoding expected = {
	  {"chunked", tp},
  };
  http::Request req;
  http::parse_headers(req.headers, message);
  http::header_analysis(req, req.headers);
  ASSERT_EQ(req.transfer_encoding, expected);
}

TEST(TestParserTransferEncoding, TransferEncoding5) {
  char message[] = "Transfer-Encoding:chunked,  chunked,  , ,  \r\n"
				   "\r\n";
  std::vector<http::transfer_parameter> tp = {};
  http::TransferEncoding expected = {
	  {"chunked", tp},
	  {"chunked", tp},
  };
  http::Request req;
  http::parse_headers(req.headers, message);
  http::header_analysis(req, req.headers);
  ASSERT_EQ(req.transfer_encoding, expected);
}

TEST(TestParserTransferEncoding, TransferEncoding6) {
  char message[] = "Transfer-Encoding:chunked ;size=45, chunked;   size  =  23\r\n"
				   "\r\n";
  std::vector<http::transfer_parameter> tp1 = {
	  {"size", "45"},
  };
  std::vector<http::transfer_parameter> tp2 = {
	  {"size", "23"},
  };
  http::TransferEncoding expected = {
	  {"chunked", tp1},
	  {"chunked", tp2},
  };
  http::Request req;
  http::parse_headers(req.headers, message);
  http::header_analysis(req, req.headers);
  ASSERT_EQ(req.transfer_encoding, expected);
}

TEST(TestParserTransferEncoding, TransferEncoding7) {
  char message[] = "Transfer-Encoding:chunkEd ;size=45, chunked;   size  =  23\r\n"
				   "\r\n";
  std::vector<http::transfer_parameter> tp1 = {
	  {"size", "45"},
  };
  std::vector<http::transfer_parameter> tp2 = {
	  {"size", "23"},
  };
  http::TransferEncoding expected = {
  	{"chunked", tp1},
  	{"chunked", tp2},
  };
  http::Request req;
  http::parse_headers(req.headers, message);
  http::header_analysis(req, req.headers);
  ASSERT_EQ(req.transfer_encoding, expected);
}

TEST(TestParserTransferEncoding, TransferEncoding8) {
  char message[] = "Transfer-Encoding:chunked ;size=\"45\", chunked;   size"
				   "  =   \"2\\\"3\"\r\n"
	   "\r\n";
  std::vector<http::transfer_parameter> tp1 = {
	  {"size", "45"},
  };
  std::vector<http::transfer_parameter> tp2 = {
	  {"size", "2\"3"},
  };
  http::TransferEncoding expected = {
  	{"chunked", tp1},
  	{"chunked", tp2},
  };
  http::Request req;
  http::parse_headers(req.headers, message);
  http::header_analysis(req, req.headers);
  ASSERT_EQ(req.transfer_encoding, expected);
}

TEST(TestParserTransferEncoding, TransferEncodingFail1) {
  char message[] = "Transfer-Encoding:gzip ;size=\"45, chunked;   size  =  23\r\n"
				   "\r\n";
  http::Request req;
  http::parse_headers(req.headers, message);
  ASSERT_ANY_THROW(http::header_analysis(req, req.headers));
}

TEST(TestParserTransferEncoding, TransferEncodingFail2) {
  char message[] = "Transfer-Encoding:gzi ;size=45, chunked;   size  =  23\r\n"
				   "\r\n";

  http::Request req;
  http::parse_headers(req.headers, message);
  ASSERT_ANY_THROW(http::header_analysis(req, req.headers));
}

TEST(TestParserTransferEncoding, TransferEncoding9) {
  char message[] = "Transfer-Encoding:chunked ;size=45 ;size=87, chunked;   size  =  "
				   "23\r\n"
				   "\r\n";
  std::vector<http::transfer_parameter> tp1 = {
	  {"size", "45"},
	  {"size", "87"}
  };
  std::vector<http::transfer_parameter> tp2 = {
	  {"size", "23"},
  };
  http::TransferEncoding expected = {
	  {"chunked", tp1},
	  {"chunked", tp2},
  };
  http::Request req;
  http::parse_headers(req.headers, message);
  http::header_analysis(req, req.headers);
  ASSERT_EQ(req.transfer_encoding, expected);
}
