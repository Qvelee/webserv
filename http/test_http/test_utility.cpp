#include "../http.hpp"
#include "../utility_http.hpp"
#include "../test_http/utility.hpp"
#include <gtest/gtest.h>

TEST(GetToken, Ok) {
  std::string expected = "GET";
  std::string current;
  http::StatusCode err = http::NoError;
  ASSERT_EQ(http::get_token(current, "GET", 0, err), 3);
  ASSERT_EQ(current, expected);
  ASSERT_EQ(err, 0);
}

TEST(GetToken, Fail1) {
  std::string current;
  http::StatusCode err = http::NoError;
  http::get_token(current, "", 0, err);
  ASSERT_EQ(err, 400);
}

TEST(GetToken, Fail2) {
  std::string current;
  http::StatusCode err = http::NoError;
  http::get_token(current, "(GET", 0, err);
  ASSERT_EQ(err, 400);
}

TEST(GetHttpVersion, Ok) {
  std::string expected = "HTTP/1.1";
  std::string current;
  http::StatusCode err = http::NoError;
  ASSERT_EQ(http::get_http_version(current, "HTTP/1.1", 0, err), 8);
  ASSERT_EQ(current, expected);
  ASSERT_EQ(err, 0);
}

TEST(GetHttpVersion, Fail1) {
  std::string current;
  http::StatusCode err = http::NoError;
  http::get_http_version(current, "HTTp/1.1", 0, err);
  ASSERT_EQ(err, 505);
}

TEST(GetHttpVersion, Fail2) {
  std::string current;
  http::StatusCode err = http::NoError;
  http::get_http_version(current, "HTTP/h.1", 0, err);
  ASSERT_EQ(err, 400);
}

TEST(GetHttpVersion, Fail3) {
  std::string current;
  http::StatusCode err = http::NoError;
  http::get_http_version(current, "HTTP/1,5", 0, err);
  ASSERT_EQ(err, 400);
}


TEST(GetHttpVersion, Fail4) {
  std::string current;
  http::StatusCode err = http::NoError;
  http::get_http_version(current, "HTTP/1.k", 0, err);
  ASSERT_EQ(err, 400);
}

TEST(SkipSpace, SP1) {
  http::StatusCode err = http::NoError;
  ASSERT_EQ(1, http::skip_space(" ", 0, http::SP, err));
  ASSERT_EQ(err, 0);
}

TEST(SkipSpace, SP2) {
  http::StatusCode err = http::NoError;
  ASSERT_EQ(1, http::skip_space("   jh", 0, http::SP, err));
  ASSERT_EQ(err, 0);
}

TEST(SkipSpace, SPFail) {
  http::StatusCode err = http::NoError;
  http::skip_space("jh", 0, http::SP, err);
  ASSERT_EQ(err, 400);
}

TEST(SkipSpace, OWS1) {
  http::StatusCode err = http::NoError;
  ASSERT_EQ(0, http::skip_space("fd", 0, http::OWS, err));
  ASSERT_EQ(err, 0);
}

TEST(SkipSpace, OWS2) {
  http::StatusCode err = http::NoError;
  ASSERT_EQ(1, http::skip_space(" fd", 0, http::OWS, err));
  ASSERT_EQ(err, 0);
}

TEST(SkipSpace, OWS3) {
  http::StatusCode err = http::NoError;
  ASSERT_EQ(3, http::skip_space("   fd", 0, http::OWS, err));
  ASSERT_EQ(err, 0);
}

TEST(SkipSpace, RWS1) {
  http::StatusCode err = http::NoError;
  ASSERT_EQ(1, http::skip_space(" fd", 0, http::RWS, err));
  ASSERT_EQ(err, 0);
}

TEST(SkipSpace, RWS2) {
  http::StatusCode err = http::NoError;
  ASSERT_EQ(3, http::skip_space("   fd", 0, http::RWS, err));
  ASSERT_EQ(err, 0);
}

TEST(SkipSpace, RWSFail) {
  http::StatusCode err = http::NoError;
  http::skip_space("jh", 0, http::RWS, err);
  ASSERT_EQ(err, 400);
}

TEST(QuotedString, Simple) {
  std::string expected = "qwerty";
  std::string current;
  http::StatusCode err = http::NoError;
  std::size_t size = http::get_quoted_string(current, "qwerty\"", 0, err);
  ASSERT_EQ(expected, current);
  ASSERT_EQ(size, 7);
  ASSERT_EQ(err, 0);
}

TEST(QuotedString, Simple2) {
  std::string expected = "	 !qwerty";
  std::string current;
  http::StatusCode err = http::NoError;
  std::size_t size = http::get_quoted_string(current, "	 !qwerty\"", 0, err);
  ASSERT_EQ(expected, current);
  ASSERT_EQ(size, 10);
  ASSERT_EQ(err, 0);
}

TEST(QuotedString, QuotedPair) {
  std::string expected = "	 \\!qwerty";
  std::string current;
  http::StatusCode err = http::NoError;
  std::size_t size = http::get_quoted_string(current, "	 \\\\!qwerty\"", 0, err);
  ASSERT_EQ(expected, current);
  ASSERT_EQ(size, 12);
  ASSERT_EQ(err, 0);
}

TEST(QuotedString, QuotedPair2) {
  std::string expected = "	 \\!qwe\"rty";
  std::string current;
  http::StatusCode err = http::NoError;
  std::size_t size = http::get_quoted_string(current, "	 \\\\!qwe\\\"rty\"", 0, err);
  ASSERT_EQ(expected, current);
  ASSERT_EQ(size, 14);
  ASSERT_EQ(err, 0);
}

TEST(QuotedString, NoDQUOTED) {
  std::string tmp;
  http::StatusCode err = http::NoError;
  http::get_quoted_string(tmp, "	 \\\\!qwe\\\"rty", 0, err);
  ASSERT_EQ(err, 400);
}

TEST(QuotedString, ForbiddenSymbol) {
  std::string tmp;
  http::StatusCode err = http::NoError;
  http::get_quoted_string(tmp, "	\r \\\\!qwe\\\"rty\"", 0, err);
  ASSERT_EQ(err, 400);
}

TEST(QuotedString, ForbiddenSymbol2) {
  std::string tmp;
  http::StatusCode err = http::NoError;
  http::get_quoted_string(tmp, "	\\\r \\\\!qwe\\\"rty\"", 0, err);
  ASSERT_EQ(err, 400);
}
