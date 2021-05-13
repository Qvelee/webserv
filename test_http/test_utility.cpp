#include "../http.hpp"
#include "../utility_http.hpp"
#include <gtest/gtest.h>

TEST(GetToken, Ok) {
  std::string expected = "GET";
  std::string current;
  std::size_t size = http::get_token(current, "GET");
  ASSERT_EQ(current, expected);
  ASSERT_EQ(size, 3);
}

TEST(GetToken, Fail1) {
  std::string current;
  ASSERT_ANY_THROW(http::get_token(current, ""));
}

TEST(GetToken, Fail2) {
  std::string current;
  ASSERT_ANY_THROW(http::get_token(current, "(GET"));
}

/*
 * Add GetRequestTargetTests
 */


TEST(GetHttpVersion, Ok) {
  std::string expected = "HTTP/1.1";
  std::string current;
  std::size_t size = http::get_http_version(current, "HTTP/1.1");
  ASSERT_EQ(current, expected);
  ASSERT_EQ(size, 8);
}

TEST(GetHttpVersion, Fail1) {
  std::string current;
  ASSERT_ANY_THROW(http::get_http_version(current, "HTTp/1.1"));
}

TEST(GetHttpVersion, Fail2) {
  std::string current;
  ASSERT_ANY_THROW(http::get_http_version(current, "HTTP/h.1"));
}

TEST(GetHttpVersion, Fail3) {
  std::string current;
  ASSERT_ANY_THROW(http::get_http_version(current, "HTTP/1,5"));
}


TEST(GetHttpVersion, Fail4) {
  std::string current;
  ASSERT_ANY_THROW(http::get_http_version(current, "HTTP/1.k"));
}

TEST(SkipSpace, SP1) {
  ASSERT_EQ(1, http::skip_space(" ", http::SP));
}

TEST(SkipSpace, SP2) {
  ASSERT_EQ(1, http::skip_space("   jh", http::SP));
}

TEST(SkipSpace, SPFail) {
  ASSERT_ANY_THROW( http::skip_space("jh", http::SP));
}

TEST(SkipSpace, OWS1) {
  ASSERT_EQ(0, http::skip_space("fd", http::OWS));
}

TEST(SkipSpace, OWS2) {
  ASSERT_EQ(1, http::skip_space(" fd", http::OWS));
}

TEST(SkipSpace, OWS3) {
  ASSERT_EQ(3, http::skip_space("   fd", http::OWS));
}

TEST(SkipSpace, RWS1) {
  ASSERT_EQ(1, http::skip_space(" fd", http::RWS));
}

TEST(SkipSpace, RWS2) {
  ASSERT_EQ(3, http::skip_space("   fd", http::RWS));
}

TEST(SkipSpace, RWSFail) {
  ASSERT_ANY_THROW( http::skip_space("jh", http::RWS));
}

TEST(QuotedString, Simple) {
  std::string expected = "qwerty";
  std::string current;
  std::size_t size = http::get_quoted_string(current, "qwerty\"");
  ASSERT_EQ(expected, current);
  ASSERT_EQ(size, 7);
}

TEST(QuotedString, Simple2) {
  std::string expected = "	 !qwerty";
  std::string current;
  std::size_t size = http::get_quoted_string(current, "	 !qwerty\"");
  ASSERT_EQ(expected, current);
  ASSERT_EQ(size, 10);
}

TEST(QuotedString, QuotedPair) {
  std::string expected = "	 \\!qwerty";
  std::string current;
  std::size_t size = http::get_quoted_string(current, "	 \\\\!qwerty\"");
  ASSERT_EQ(expected, current);
  ASSERT_EQ(size, 12);
}

TEST(QuotedString, QuotedPair2) {
  std::string expected = "	 \\!qwe\"rty";
  std::string current;
  std::size_t size = http::get_quoted_string(current, "	 \\\\!qwe\\\"rty\"");
  ASSERT_EQ(expected, current);
  ASSERT_EQ(size, 14);
}

TEST(QuotedString, NoDQUOTED) {
  std::string tmp;
  ASSERT_ANY_THROW(http::get_quoted_string(tmp, "	 \\\\!qwe\\\"rty"));
}

TEST(QuotedString, ForbiddenSymbol) {
  std::string tmp;
  ASSERT_ANY_THROW(http::get_quoted_string(tmp, "	\r \\\\!qwe\\\"rty\""));
}

TEST(QuotedString, ForbiddenSymbol2) {
  std::string tmp;
  ASSERT_ANY_THROW(http::get_quoted_string(tmp, "	\\\r \\\\!qwe\\\"rty\""));
}
