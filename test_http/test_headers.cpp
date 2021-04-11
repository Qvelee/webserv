#include "../http.hpp"
#include <gtest/gtest.h>


std::ostream&
operator<<(std::ostream& os, const http::message::transfer_parameter& tp) {
  os << tp.name_ << "=" << tp.value_;
  return os;
}

std::ostream&
operator<<(std::ostream& os, const http::message::transfer_extension& te) {
  os << te.token_ << ";" << te.transfer_parameter_;
  return os;
}

std::ostream&
operator<<(std::ostream& os, const http::message::message_info& rl) {
  os << rl.content_length_ << "\n";
  os << rl.content_length_ << "\n";
  for (auto& i : rl.transfer_coding_)
    os << i << "\n";
  return os;
}


TEST(TestParserHeaders, SimpleMessage) {
  char message[] = "field1:value1\r\n"
	   			   "field2:value2\r\n"
		  		   "field3:value3\r\n"
		           "\r\n";

  std::map<std::string, std::string> expected;
  expected["field1"] = "value1";
  expected["field2"] = "value2";
  expected["field3"] = "value3";

  std::map<std::string, std::string> current;
  std::size_t size = http::message::parse_headers(current, message);
  ASSERT_EQ(size, strlen(message));
  ASSERT_EQ(current, expected);
}

TEST(TestParserHeaders, NoSemicolons) {
  char message[] = "field1;value1\r\n"
				   "field2:value2\r\n"
				   "field3:value3\r\n"
				   "\r\n";

  std::map<std::string, std::string> current;
  ASSERT_ANY_THROW(http::message::parse_headers(current, message));
}

TEST(TestParserHeaders, SpaceAroundFieldValue) {
  char message[] = "field1: value1 \r\n"
				   "field2:  value2  \r\n"
	   			   "field3:  value3  \r\n"
		  		   "\r\n";

  std::map<std::string, std::string> expected;
  expected["field1"] = "value1";
  expected["field2"] = "value2";
  expected["field3"] = "value3";

  std::map<std::string, std::string> current;
  std::size_t size = http::message::parse_headers(current, message);
  ASSERT_EQ(size, strlen(message));
  ASSERT_EQ(current, expected);
}

TEST(TestParserHeaders, SpaceIntoFieldValue) {
  char message[] = "field1:va lue1\r\n"
	   			   "field2:value2\r\n"
		  		   "field3:va lu  e3\r\n"
		 		   "\r\n";

  std::map<std::string, std::string> expected;
  expected["field1"] = "va lue1";
  expected["field2"] = "value2";
  expected["field3"] = "va lu  e3";

  std::map<std::string, std::string> current;
  std::size_t size = http::message::parse_headers(current, message);
  ASSERT_EQ(size, strlen(message));
  ASSERT_EQ(current, expected);
}

TEST(TestParserHeaders, ObsFold) {
  char message[] =  "field1:val\r\n   ue1\r\n"
	   				"field2:v\r\n \talu\r\n e2\r\n"
					"field3:value3\r\n"
	 				"\r\n";

  std::map<std::string, std::string> expected;
  expected["field1"] = "val ue1";
  expected["field2"] = "v alu e2";
  expected["field3"] = "value3";

  std::map<std::string, std::string> current;
  std::size_t size = http::message::parse_headers(current, message);
  ASSERT_EQ(size, strlen(message));
  ASSERT_EQ(current, expected);
}

TEST(TestParserHeaders, ForbiddenSymbol) {
  char message[] = "field1:va\nue1\r\n"
				   "field2:value2\r\n"
				   "field3:value3\r\n"
				   "\r\n";

  std::map<std::string, std::string> current;
  ASSERT_ANY_THROW(http::message::parse_headers(current, message));
}


TEST(TestParserHeaders, TwoSameNames) {
  char message[] = 	 "field1:value1\r\n"
					 "field2:val ue2\r\n"
					 "field3:value3\r\n"
	  				 "field1:value4\r\n"
					 "\r\n";

  std::map<std::string, std::string> expected;
  expected["field1"] = "value1,value4";
  expected["field2"] = "val ue2";
  expected["field3"] = "value3";

  std::map<std::string, std::string> current;
  std::size_t size = http::message::parse_headers(current, message);
  ASSERT_EQ(size, strlen(message));
  ASSERT_EQ(current, expected);
}

TEST(TestParserHeaders, SpaceAfterStartLine) {
  char message[] = " \r\n"
				   "field1:value1";

  std::map<std::string, std::string> current;
  ASSERT_ANY_THROW(http::message::parse_headers(current, message));
}

TEST(TestParserHeaders, SpaceAfterFieldName) {
  char message[] = 	 "field1  :value1\r\n"
					 "field2:value2\r\n"
					 "field3:value3\r\n"
					 "\r\n";

  std::map<std::string, std::string> current;
  ASSERT_ANY_THROW(http::message::parse_headers(current, message));
}

TEST(TestParserHeaders, NoCRLF) {
  char message[] = 	 "field1:value1\r\n"
					 "field2:val ue2\r\n"
					 "field3:value3\r\n"
					 "field1:value4\r\n";

  std::map<std::string, std::string> current;
  ASSERT_ANY_THROW(http::message::parse_headers(current, message));
}

TEST(TestParserHeaders, CaseSensitive) {
  char message[] =	"FIELD1:value1\r\n"
					 "fieLd2:val ue2\r\n"
					 "field3:value3\r\n"
					 "field1:value4\r\n"
					 "\r\n";

  std::map<std::string, std::string> expected;
  expected["field1"] = "value1,value4";
  expected["field2"] = "val ue2";
  expected["field3"] = "value3";

  std::map<std::string, std::string> current;
  std::size_t size = http::message::parse_headers(current, message);
  ASSERT_EQ(size, strlen(message));
  ASSERT_EQ(current, expected);
}

// content-length
TEST(TestParserContentLength, ContentLength) {
  char message[] = "Content-Length:42\r\n"
	               "\r\n";
  http::message current;
  http::message::parse_headers(current.headers_, message);
  current.content_length();
  ASSERT_EQ(42, current.message_info_.content_length_);
}

TEST(TestParserContentLength, TwoHeadersWithContentLength) {
  char message[] = "Content-Length:42\r\n"
				   "Content-Length:42\r\n"
				   "\r\n";
  http::message current;
  http::message::parse_headers(current.headers_, message);
  ASSERT_ANY_THROW(current.content_length());
}

TEST(TestParserContentLength, TwoValueInContentLength) {
  char message[] = "Content-Length:42,42\r\n"
				   "\r\n";
  http::message current;
  http::message::parse_headers(current.headers_, message);
  ASSERT_ANY_THROW(current.content_length());
}

TEST(TestParserContentLength, MoreValueInContentLength) {
  char message[] = "Content-Length:42, 42,,  ,   42  \r\n"
				   "\r\n";
  http::message current;
  http::message::parse_headers(current.headers_, message);
  ASSERT_ANY_THROW(current.content_length());
}

TEST(TestParserContentLength, DifferentValueInContentLength) {
  char message[] = "Content-Length:42\r\n"
				   "Content-Length:43\r\n"
				   "\r\n";
  http::message current;
  http::message::parse_headers(current.headers_, message);
  ASSERT_ANY_THROW(current.content_length());
}

TEST(TestParserContentLength, DifferentValueInOneHeader) {
  char message[] = "Content-Length:42, 43\r\n"
				   "\r\n";
  http::message current;
  http::message::parse_headers(current.headers_, message);
  ASSERT_ANY_THROW(current.content_length());
}

TEST(TestParserContentLength, EmptyContentLength) {
  char message[] = "Content-Length:  , , \r\n"
				   "\r\n";
  http::message current;
  http::message::parse_headers(current.headers_, message);
  ASSERT_ANY_THROW(current.content_length());
}

TEST(TestParserContentLength, BigNumber) {
  char message[] = "Content-Length:99999999999999999999999\r\n"
				   "\r\n";
  http::message current;
  http::message::parse_headers(current.headers_, message);
  ASSERT_ANY_THROW(current.content_length());
}

// transfer-encoding
TEST(TestParserTransferEncoding, TransferEncoding) {
  char message[] = "Transfer-Encoding:gzip\r\n"
				   "\r\n";
  std::vector<http::message::transfer_extension> expected = {
	  {"gzip", {"", ""}},
  };
  http::message current;
  http::message::parse_headers(current.headers_, message);
  current.transfer_encoding();
  ASSERT_EQ(current.message_info_.transfer_coding_, expected);
}

TEST(TestParserTransferEncoding, TransferEncoding2) {
  char message[] = "Transfer-Encoding: gzip  ;  size=1\r\n"
				   "\r\n";
  std::vector<http::message::transfer_extension> expected = {
	  {"gzip", {"size", "1"}},
  };
  http::message current;
  http::message::parse_headers(current.headers_, message);
  current.transfer_encoding();
  ASSERT_EQ(current.message_info_.transfer_coding_, expected);
}

TEST(TestParserTransferEncoding, TransferEncoding3) {
  char message[] = "Transfer-Encoding:gzip  ;  size   =   1  \r\n"
				   "\r\n";
  std::vector<http::message::transfer_extension> expected = {
	  {"gzip", {"size", "1"}},
  };
  http::message current;
  http::message::parse_headers(current.headers_, message);
  current.transfer_encoding();
  ASSERT_EQ(current.message_info_.transfer_coding_, expected);
}

TEST(TestParserTransferEncoding, TransferEncoding4) {
  char message[] = "Transfer-Encoding:gzip;size=1\r\n"
				   "\r\n";
  std::vector<http::message::transfer_extension> expected = {
	  {"gzip", {"size", "1"}},
  };
  http::message current;
  http::message::parse_headers(current.headers_, message);
  current.transfer_encoding();
  ASSERT_EQ(current.message_info_.transfer_coding_, expected);
}

TEST(TestParserTransferEncoding, TransferEncoding5) {
  char message[] = "Transfer-Encoding:gzip,  chunked,  , ,  \r\n"
				   "\r\n";
  std::vector<http::message::transfer_extension> expected = {
	  {"gzip", {"", ""}},
	  {"chunked", {"", ""},}
  };
  http::message current;
  http::message::parse_headers(current.headers_, message);
  current.transfer_encoding();
  ASSERT_EQ(current.message_info_.transfer_coding_, expected);
}

TEST(TestParserTransferEncoding, TransferEncoding6) {
  char message[] = "Transfer-Encoding:gzip ;size=45, chunked;   size  =  23\r\n"
				   "\r\n";
  std::vector<http::message::transfer_extension> expected = {
	  {"gzip", {"size", "45"}},
	  {"chunked", {"size", "23"},}
  };
  http::message current;
  http::message::parse_headers(current.headers_, message);
  current.transfer_encoding();
  ASSERT_EQ(current.message_info_.transfer_coding_, expected);
}

TEST(TestParserTransferEncoding, TransferEncoding7) {
  char message[] = "Transfer-Encoding:GZIp ;size=45, chunked;   size  =  23\r\n"
				   "\r\n";
  std::vector<http::message::transfer_extension> expected = {
	  {"gzip", {"size", "45"}},
	  {"chunked", {"size", "23"},}
  };
  http::message current;
  http::message::parse_headers(current.headers_, message);
  current.transfer_encoding();
  ASSERT_EQ(current.message_info_.transfer_coding_, expected);
}

TEST(TestParserTransferEncoding, TransferEncoding8) {
  char message[] = "Transfer-Encoding:gzip ;size=\"45\", chunked;   size"
				   "  =   \"2\\\"3\"\r\n"
				   "\r\n";
  std::vector<http::message::transfer_extension> expected = {
	  {"gzip", {"size", "45"}},
	  {"chunked", {"size", "2\"3"},}
  };
  http::message current;
  http::message::parse_headers(current.headers_, message);
  current.transfer_encoding();
  ASSERT_EQ(current.message_info_.transfer_coding_, expected);
}

TEST(TestParserTransferEncoding, TransferEncodingFail1) {
  char message[] = "Transfer-Encoding:gzip ;size=\"45, chunked;   size  =  "
				   "23\r\n"
				   "\r\n";
  http::message current;
  http::message::parse_headers(current.headers_, message);
  ASSERT_ANY_THROW(current.transfer_encoding());
}

TEST(TestParserTransferEncoding, TransferEncodingFail2) {

  char message[] = "Transfer-Encoding:gzi ;size=45, chunked;   size  =  "
				   "23\r\n"
				   "\r\n";
  http::message current;
  http::message::parse_headers(current.headers_, message);
  ASSERT_ANY_THROW(current.transfer_encoding());
}
