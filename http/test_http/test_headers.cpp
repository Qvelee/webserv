#include "../http.hpp"
#include <gtest/gtest.h>


std::ostream&
operator<<(std::ostream& os, const http::transfer_parameter& tp) {
  os << tp.name << "=" << tp.value;
  return os;
}

std::ostream&
operator<<(std::ostream& os, const http::transfer_extension& te) {
  os << te.token;
  for (auto &it : te.transfer_parameter) {
    os << ";" << it;
  }
  return os;
}



TEST(TestParserHeaders, SpaceAfterStartLine) {
  char message[] = " \r\n"
				   "field1:value1\r\n";

  std::map<std::string, std::string> current;
  ASSERT_ANY_THROW(http::parse_headers(current, message));
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
  std::size_t size = http::parse_headers(current, message);
  ASSERT_EQ(size, strlen(message));
  ASSERT_EQ(current, expected);
}

TEST(TestParserHeaders, NoSemicolons) {
  char message[] = "field1;value1\r\n"
				   "field2:value2\r\n"
				   "field3:value3\r\n"
				   "\r\n";

  std::map<std::string, std::string> current;
  ASSERT_ANY_THROW(http::parse_headers(current, message));
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
  std::size_t size = http::parse_headers(current, message);
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
  std::size_t size = http::parse_headers(current, message);
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
  std::size_t size = http::parse_headers(current, message);
  ASSERT_EQ(size, strlen(message));
  ASSERT_EQ(current, expected);
}

TEST(TestParserHeaders, ForbiddenSymbol) {
  char message[] = "field1:va\nue1\r\n"
				   "field2:value2\r\n"
				   "field3:value3\r\n"
				   "\r\n";

  std::map<std::string, std::string> current;
  ASSERT_ANY_THROW(http::parse_headers(current, message));
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
  std::size_t size = http::parse_headers(current, message);
  ASSERT_EQ(size, strlen(message));
  ASSERT_EQ(current, expected);
}



TEST(TestParserHeaders, SpaceAfterFieldName) {
  char message[] = 	 "field1  :value1\r\n"
					 "field2:value2\r\n"
					 "field3:value3\r\n"
					 "\r\n";

  std::map<std::string, std::string> current;
  ASSERT_ANY_THROW(http::parse_headers(current, message));
}

TEST(TestParserHeaders, NoCRLF) {
  char message[] = 	 "field1:value1\r\n"
					 "field2:val ue2\r\n"
					 "field3:value3\r\n"
					 "field1:value4\r\n";

  std::map<std::string, std::string> current;
  ASSERT_ANY_THROW(http::parse_headers(current, message));
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
  std::size_t size = http::parse_headers(current, message);
  ASSERT_EQ(size, strlen(message));
  ASSERT_EQ(current, expected);
}
