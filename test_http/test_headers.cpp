#include "../http.hpp"
#include <gtest/gtest.h>


std::ostream& operator<<(std::ostream& os, const
http::message::transfer_parameter& tp) {
  os << tp.name_ << "=" << tp.value_;
  return os;
}

std::ostream& operator<<(std::ostream& os, const
http::message::transfer_extension& te) {
  os << te.token_ << ";" << te.transfer_parameter_;
  return os;
}

std::ostream& operator<<(std::ostream& os, const http::message::message_info&
rl) {
  os << rl.content_length_ << "\n";
  for (auto& i : rl.transfer_coding_)
    os << i << "\n";
  return os;
}


TEST(TestParserHeaders, SimpleMessage) {
	char message[] = "GET /me HTTP/1.1\r\n"
				  "field1:value1\r\n"
	  			  "field2:value2\r\n"
                  "field3:value3\r\n"
	              "\r\n";

	std::map<std::string, std::string> expected;
	expected["field1"] = "value1";
	expected["field2"] = "value2";
	expected["field3"] = "value3";

	http::message current(message);
	ASSERT_EQ(current.headers_, expected);
}

TEST(TestParserHeaders, SpaceAroundFieldValue) {
	char message[] = "GET /me HTTP/1.1\r\n"
					 "field1: value1 \r\n"
					 "field2:  value2  \r\n"
					 "field3:  value3  \r\n"
					 "\r\n";

	std::map<std::string, std::string> expected;
	expected["field1"] = "value1";
	expected["field2"] = "value2";
	expected["field3"] = "value3";

	http::message current(message);
	ASSERT_EQ(current.headers_, expected);
}

TEST(TestParserHeaders, SpaceIntoFieldValue) {
	char message[] = "GET /me HTTP/1.1\r\n"
					 "field1:va lue1\r\n"
					 "field2:value2\r\n"
					 "field3:va lu  e3\r\n"
					 "\r\n";

	std::map<std::string, std::string> expected;
	expected["field1"] = "va lue1";
	expected["field2"] = "value2";
	expected["field3"] = "va lu  e3";

	http::message current(message);
	ASSERT_EQ(current.headers_, expected);
}

TEST(TestParserHeaders, ObsFold) {
	char message[] = "GET /me HTTP/1.1\r\n"
					 "field1:val\r\n   ue1\r\n"
					 "field2:v\r\n \talu\r\n e2\r\n"
					 "field3:value3\r\n"
					 "\r\n";

	std::map<std::string, std::string> expected;
	expected["field1"] = "val ue1";
	expected["field2"] = "v alu e2";
	expected["field3"] = "value3";

	http::message current(message);
	ASSERT_EQ(current.headers_, expected);
}

TEST(TestParserHeaders, TwoSameNames) {
	char message[] = "GET /me HTTP/1.1\r\n"
					 "field1:value1\r\n"
					 "field2:val ue2\r\n"
					 "field3:value3\r\n"
	  				 "field1:value4\r\n"
					 "\r\n";

	std::map<std::string, std::string> expected;
	expected["field1"] = "value1,value4";
	expected["field2"] = "val ue2";
	expected["field3"] = "value3";

	http::message current(message);
	ASSERT_EQ(current.headers_, expected);
}

TEST(TestParserHeaders, SpaceAfterStartLine) {
	char message[] = "GET /me HTTP/1.1\r\n \r\n";
	ASSERT_ANY_THROW(http::message current(message));
}

TEST(TestParserHeaders, SpaceAfterFieldName) {
	char message[] = "GET /me HTTP/1.1\r\n"
					 "field1  :value1\r\n"
					 "field2:value2\r\n"
					 "field3:value3\r\n"
					 "\r\n";

	ASSERT_ANY_THROW(http::message current(message));
}

TEST(TestParserHeaders, NoCRLF) {
	char message[] = "GET /me HTTP/1.1\r\n"
					 "field1:value1\r\n"
					 "field2:val ue2\r\n"
					 "field3:value3\r\n"
					 "field1:value4\r\n";

	ASSERT_ANY_THROW(http::message current(message));
}

TEST(TestParserHeaders, CaseSensitive) {
	char message[] = "GET /me HTTP/1.1\r\n"
					 "FIELD1:value1\r\n"
					 "fieLd2:val ue2\r\n"
					 "field3:value3\r\n"
					 "field1:value4\r\n"
					 "\r\n";

	std::map<std::string, std::string> expected;
	expected["field1"] = "value1,value4";
	expected["field2"] = "val ue2";
	expected["field3"] = "value3";

	http::message current(message);
	ASSERT_EQ(current.headers_, expected);
}

// content-length
TEST(TestParserHeaders, ContentLength) {
  char message[] = "GET /me HTTP/1.1\r\n"
				   "Content-Length:42\r\n"
	               "\r\n";
  http::message::message_info expected = {.content_length_ = 42};
  http::message current(message);
  ASSERT_EQ(expected, current.message_info_);
}

TEST(TestParserHeaders, TwoHeadersWithContentLength) {
  char message[] = "GET /me HTTP/1.1\r\n"
				   "Content-Length:42\r\n"
				   "Content-Length:42\r\n"
				   "\r\n";
  ASSERT_ANY_THROW(http::message current(message));
}

TEST(TestParserHeaders, TwoValueInContentLength) {
  char message[] = "GET /me HTTP/1.1\r\n"
				   "Content-Length:42,42\r\n"
				   "Content-Length:42,\r\n"
				   "\r\n";
  ASSERT_ANY_THROW(http::message current(message));
}

TEST(TestParserHeaders, MoreValueInContentLength) {
  char message[] = "GET /me HTTP/1.1\r\n"
				   "Content-Length:42, 42,,  ,   42  \r\n"
				   "\r\n";
  ASSERT_ANY_THROW(http::message current(message));
}

TEST(TestParserHeaders, DifferentValueInContentLength) {
  char message[] = "GET /me HTTP/1.1\r\n"
				   "Content-Length:42\r\n"
				   "Content-Length:43\r\n"
				   "\r\n";
  ASSERT_ANY_THROW(http::message current(message));
}

TEST(TestParserHeaders, DifferentValueInOneHeader) {
  char message[] = "GET /me HTTP/1.1\r\n"
				   "Content-Length:42, 43\r\n"
				   "\r\n";
  ASSERT_ANY_THROW(http::message current(message));
}

TEST(TestParserHeaders, EmptyContentLength) {
  char message[] = "GET /me HTTP/1.1\r\n"
				   "Content-Length:  , , \r\n"
				   "\r\n";
  ASSERT_ANY_THROW(http::message current(message));
}

TEST(TestParserHeaders, BigNumber) {
  char message[] = "GET /me HTTP/1.1\r\n"
				   "Content-Length:99999999999999999999999\r\n"
				   "\r\n";
  ASSERT_ANY_THROW(http::message current(message));
}

// transfer-encoding
TEST(TestParserHeaders, TransferEncoding) {
  {
	char message[] = "GET /me HTTP/1.1\r\n"
					 "Transfer-Encoding:gzip\r\n"
					 "\r\n";
	std::vector<http::message::transfer_extension> expected = {
		{"gzip", {"", ""}},
	};
	http::message current(message);
	ASSERT_EQ(current.message_info_.transfer_coding_, expected);
  }

  {
	char message[] = "GET /me HTTP/1.1\r\n"
					 "Transfer-Encoding: gzip  ;  size=1\r\n"
					 "\r\n";
	std::vector<http::message::transfer_extension> expected = {
		{"gzip", {"size", "1"}},
	};
	http::message current(message);
	ASSERT_EQ(current.message_info_.transfer_coding_, expected);
  }

  {
	char message[] = "GET /me HTTP/1.1\r\n"
					 "Transfer-Encoding:gzip  ;  size   =   1  \r\n"
					 "\r\n";
	std::vector<http::message::transfer_extension> expected = {
		{"gzip", {"size", "1"}},
	};
	http::message current(message);
	ASSERT_EQ(current.message_info_.transfer_coding_, expected);
  }

  {
	char message[] = "GET /me HTTP/1.1\r\n"
					 "Transfer-Encoding:gzip;size=1\r\n"
					 "\r\n";
	std::vector<http::message::transfer_extension> expected = {
		{"gzip", {"size", "1"}},
	};
	http::message current(message);
	ASSERT_EQ(current.message_info_.transfer_coding_, expected);
  }

  {
	char message[] = "GET /me HTTP/1.1\r\n"
					 "Transfer-Encoding:gzip,  chunked,  , ,  \r\n"
					 "\r\n";
	std::vector<http::message::transfer_extension> expected = {
		{"gzip", {"", ""}},
		{"chunked", {"", ""},}
	};
	http::message current(message);
	ASSERT_EQ(current.message_info_.transfer_coding_, expected);
  }

  {
	char message[] = "GET /me HTTP/1.1\r\n"
					 "Transfer-Encoding:gzip ;size=45, chunked;   size  =  23\r\n"
					 "\r\n";
	std::vector<http::message::transfer_extension> expected = {
		{"gzip", {"size", "45"}},
		{"chunked", {"size", "23"},}
	};
	http::message current(message);
	ASSERT_EQ(current.message_info_.transfer_coding_, expected);
  }

  {
	char message[] = "GET /me HTTP/1.1\r\n"
					 "Transfer-Encoding:GZIp ;size=45, chunked;   size  =  "
	  "23\r\n"
					 "\r\n";
	std::vector<http::message::transfer_extension> expected = {
		{"gzip", {"size", "45"}},
		{"chunked", {"size", "23"},}
	};
	http::message current(message);
	ASSERT_EQ(current.message_info_.transfer_coding_, expected);
  }

  {
	char message[] = "GET /me HTTP/1.1\r\n"
				  "Transfer-Encoding:gzip ;size=\"45\", chunked;   size"
	  "  =   \"2\\\"3\"\r\n"
					 "\r\n";
	std::vector<http::message::transfer_extension> expected = {
		{"gzip", {"size", "45"}},
		{"chunked", {"size", "2\"3"},}
	};
	http::message current(message);
	ASSERT_EQ(current.message_info_.transfer_coding_, expected);
  }
}

TEST(TestParserHeaders, TransferEncodingFail) {
  {
	char message[] = "GET /me HTTP/1.1\r\n"
					 "Transfer-Encoding:gzip ;size=\"45, chunked;   size  =  "
	  "23\r\n"
					 "\r\n";
	ASSERT_ANY_THROW(http::message current(message));
  }

  {
	char message[] = "GET /me HTTP/1.1\r\n"
					 "Transfer-Encoding:gzi ;size=45, chunked;   size  =  "
					 "23\r\n"
					 "\r\n";
	ASSERT_ANY_THROW(http::message current(message));
  }

}
