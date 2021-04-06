#include "../http.hpp"
#include <gtest/gtest.h>


TEST(TestParserHeaders, FailSpaceAfterStartLine) {
	char message[] = "GET /me HTTP/1.1\r\n \r\n";

	ASSERT_ANY_THROW(http::message current(message));
}

TEST(TestParserHeaders, NoFail1) {
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

TEST(TestParserHeaders, NoFail2) {
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

TEST(TestParserHeaders, NoFail3) {
	char message[] = "GET /me HTTP/1.1\r\n"
					 "field1:val\r\n   ue1\r\n"
					 "field2:v\r\n \talu\r\n e2\r\n"
					 "field3:value3\r\n"
					 "\r\n";

	std::map<std::string, std::vector<std::string> > expected;

	expected["field1"] = {"value1"};
	expected["field2"] = {"value2"};
	expected["field3"] = {"value3"};

	http::message current(message);

	ASSERT_EQ(current.headers_, expected);
}

TEST(TestParserHeaders, NoFail4) {
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
