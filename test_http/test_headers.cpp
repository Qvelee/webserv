#include "../http.hpp"
#include <gtest/gtest.h>


std::ostream& operator<<(std::ostream& os, const http::message::message_info&
rl) {
  os << rl.content_length << "";
  return os;
}


TEST(TestParserHeaders, SimpleMessage) {
	char message[] = "GET /me HTTP/1.1\r\n"
				  "field1:value1\r\n"
	  			  "field2:value2\r\n"
                  "field3:value3\r\n"
	              "\r\n";

	std::map<std::string, std::vector<std::string> > expected;
	expected["field1"] = {"value1"};
	expected["field2"] = {"value2"};
	expected["field3"] = {"value3"};

	http::message current(message);
	ASSERT_EQ(current.headers_, expected);
}

TEST(TestParserHeaders, SpaceAroundFieldValue) {
	char message[] = "GET /me HTTP/1.1\r\n"
					 "field1: value1 \r\n"
					 "field2:  value2  \r\n"
					 "field3:  value3  \r\n"
					 "\r\n";

	std::map<std::string, std::vector<std::string> > expected;
	expected["field1"] = {"value1"};
	expected["field2"] = {"value2"};
	expected["field3"] = {"value3"};

	http::message current(message);
	ASSERT_EQ(current.headers_, expected);
}

TEST(TestParserHeaders, SpaceIntoFieldValue) {
	char message[] = "GET /me HTTP/1.1\r\n"
					 "field1:va lue1\r\n"
					 "field2:value2\r\n"
					 "field3:va lu  e3\r\n"
					 "\r\n";

	std::map<std::string, std::vector<std::string> > expected;
	expected["field1"] = {"va lue1"};
	expected["field2"] = {"value2"};
	expected["field3"] = {"va lu  e3"};

	http::message current(message);
	ASSERT_EQ(current.headers_, expected);
}

TEST(TestParserHeaders, ObsFold) {
	char message[] = "GET /me HTTP/1.1\r\n"
					 "field1:val\r\n   ue1\r\n"
					 "field2:v\r\n \talu\r\n e2\r\n"
					 "field3:value3\r\n"
					 "\r\n";

	std::map<std::string, std::vector<std::string> > expected;
	expected["field1"] = {"val ue1"};
	expected["field2"] = {"v alu e2"};
	expected["field3"] = {"value3"};

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

	std::map<std::string, std::vector<std::string> > expected;
	expected["field1"] = {"value1", "value4"};
	expected["field2"] = {"val ue2"};
	expected["field3"] = {"value3"};

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

TEST(TestParserHeaders, Fail1) {
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

	std::map<std::string, std::vector<std::string> > expected;
	expected["field1"] = {"value1", "value4"};
	expected["field2"] = {"val ue2"};
	expected["field3"] = {"value3"};

	http::message current(message);
	ASSERT_EQ(current.headers_, expected);
}

TEST(TestParserHeaders, ContentLength) {
  char message[] = "GET /me HTTP/1.1\r\n"
				   "Content-Length:42\r\n"
	               "\r\n";
  http::message::message_info expected = {.content_length = 42};
  http::message current(message);
  ASSERT_EQ(expected, current.message_info_);
}

TEST(TestParserHeaders, ContentLength2) {
  char message[] = "GET /me HTTP/1.1\r\n"
				   "Content-Length:42\r\n"
				   "Content-Length:42\r\n"
				   "\r\n";
  http::message::message_info expected = {.content_length = 42};
  http::message current(message);
  ASSERT_EQ(expected, current.message_info_);
}

TEST(TestParserHeaders, ContentLength3) {
  char message[] = "GET /me HTTP/1.1\r\n"
				   "Content-Length:42,42\r\n"
				   "Content-Length:42,\r\n"
				   "\r\n";
  http::message::message_info expected = {.content_length = 42};
  http::message current(message);
  ASSERT_EQ(expected, current.message_info_);
}

TEST(TestParserHeaders, ContentLength4) {
  char message[] = "GET /me HTTP/1.1\r\n"
				   "Content-Length:42, 42,,  ,   42  \r\n"
				   "Content-Length:42\r\n"
				   "\r\n";
  http::message::message_info expected = {.content_length = 42};
  http::message current(message);
  ASSERT_EQ(expected, current.message_info_);
}

TEST(TestParserHeaders, ContentLengthFail1) {
  char message[] = "GET /me HTTP/1.1\r\n"
				   "Content-Length:42\r\n"
				   "Content-Length:43\r\n"
				   "\r\n";
  ASSERT_ANY_THROW(http::message current(message));
}

TEST(TestParserHeaders, ContentLengthFail2) {
  char message[] = "GET /me HTTP/1.1\r\n"
				   "Content-Length:42, 43\r\n"
				   "\r\n";
  ASSERT_ANY_THROW(http::message current(message));
}

TEST(TestParserHeaders, ContentLengthFail3) {
  char message[] = "GET /me HTTP/1.1\r\n"
				   "Content-Length:  , , \r\n"
				   "\r\n";
  ASSERT_ANY_THROW(http::message current(message));
}
