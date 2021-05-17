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

TEST(GetPctEncode, Simple) {
  std::string str("");
  ASSERT_EQ(http::isPctEncoded(str, 0), false);
}

TEST(GetPctEncode, Simple2) {
  std::string str("%");
  ASSERT_EQ(http::isPctEncoded(str, 0), false);
}

TEST(GetPctEncode, Simple3) {
  std::string str("ffg");
  ASSERT_EQ(http::isPctEncoded(str, 0), false);
}

TEST(GetPctEncode, Simple4) {
  std::string str("%fu");
  ASSERT_EQ(http::isPctEncoded(str, 0), false);
}

TEST(GetPctEncode, Simple5) {
  std::string str("%fe");
  ASSERT_EQ(http::isPctEncoded(str, 0), true);
}

TEST(decodePCT, one) {
  std::string str("Hello%20world%21%0A");
  ASSERT_EQ(http::decodePCT(str), "Hello world!\n");
}

TEST(originForm, one) {
  std::string str("/hello%20world/text.txt");
  http::URL current;
  http::URL expected = {
  	.path = "/hello world/text.txt",
  	.raw_path = str,
  };
  http::parse_origin_form(current, str);
  ASSERT_EQ(current, expected);
}

TEST(originForm, two) {
  std::string str("/text.txt");
  http::URL current;
  http::URL expected = {
	  .path = "/text.txt",
  };
  http::parse_origin_form(current, str);
  ASSERT_EQ(current, expected);
}

TEST(originForm, three) {
  std::string str("/he[lo%20world/text.txt");
  http::URL current;
  ASSERT_ANY_THROW(http::parse_origin_form(current, str));
}

TEST(originForm, four) {
  std::string str("/text.txt?param1=one&param2=two");
  http::URL current;
  http::URL expected = {
	  .path = "/text.txt",
	  .raw_query = "param1=one&param2=two",
  };
  http::parse_origin_form(current, str);
  ASSERT_EQ(current, expected);
}

TEST(GetUserInfo, one) {
  std::string str("gmorros:password");
  std::string current;
  size_t i = http::get_userinfo(current, str + "@", 0);
  ASSERT_EQ(current, str);
  ASSERT_EQ(i, str.length() + 1);
}

TEST(GetUserInfo, two) {
  std::string str("");
  std::string current;
  size_t i = http::get_userinfo(current, str, 0);
  ASSERT_EQ(current, str);
  ASSERT_EQ(i, 0);
}

TEST(GetUserInfo, three) {
  std::string str("[gmorros@");
  std::string current;
  ASSERT_ANY_THROW(http::get_userinfo(current, str, 0));
}

TEST(IPv4, one) {
  ASSERT_EQ(http::isIPv4("255.2.134.11"), true);
}

TEST(IPv4, two) {
  ASSERT_EQ(http::isIPv4("255.2.134.234"), true);
}

TEST(IPv4, three) {
  ASSERT_EQ(http::isIPv4("255.2.134.269"), false);
}

TEST(IPv4, four) {
  ASSERT_EQ(http::isIPv4("259.2.134.200"), false);
}

TEST(IpLiteral, future1) {
  std::string str("vf5e4.sgds");
  std::string current;
  size_t i = http::get_ip_literal(current, str + "]", 0);
  ASSERT_EQ(current, str);
  ASSERT_EQ(i, 11);
}

TEST(IpLiteral, future2) {
  std::string str("f5e4.sgds");
  std::string current;
  ASSERT_ANY_THROW(http::get_ip_literal(current, str + "]", 0));
}

TEST(IpLiteral, future3) {
  std::string str("v.sgds");
  std::string current;
  ASSERT_ANY_THROW(http::get_ip_literal(current, str + "]", 0));
}

TEST(IpLiteral, future4) {
  std::string str("vfe");
  std::string current;
  ASSERT_ANY_THROW(http::get_ip_literal(current, str + "]", 0));
}

TEST(IpLiteral, future5) {
  std::string str("vfe.");
  std::string current;
  ASSERT_ANY_THROW(http::get_ip_literal(current, str + "]", 0));
}

TEST(IPv6, one) {
  std::string str("ff:ee:ff:ff:ff:ff:ff:ff");
  std::string current;
  size_t i = http::get_ip_literal(current, str +"]", 0);
  ASSERT_EQ(current, str);
  ASSERT_EQ(i, str.length() + 1);
}

TEST(IPv6, two) {
  std::string str("::ee:ff:ff:ff:ff:ff:ff");
  std::string current;
  size_t i = http::get_ip_literal(current, str +"]", 0);
  ASSERT_EQ(current, str);
  ASSERT_EQ(i, str.length() + 1);
}

TEST(IPv6, three) {
  std::string str("::ff:ff:ff:ff:ff:ff");
  std::string current;
  size_t i = http::get_ip_literal(current, str +"]", 0);
  ASSERT_EQ(current, str);
  ASSERT_EQ(i, str.length() + 1);
}

TEST(IPv6, four) {
  std::string str("::ff:ff:ff:255.244.244.7");
  std::string current;
  size_t i = http::get_ip_literal(current, str +"]", 0);
  ASSERT_EQ(current, str);
  ASSERT_EQ(i, str.length() + 1);
}

TEST(IPv6, fife) {
  std::string str("ff:ff::ff:ff:ff:255.244.244.7");
  std::string current;
  size_t i = http::get_ip_literal(current, str +"]", 0);
  ASSERT_EQ(current, str);
  ASSERT_EQ(i, str.length() + 1);
}

TEST(IPv6, six) {
  std::string str("ff:ff:ff:ff:ff::");
  std::string current;
  size_t i = http::get_ip_literal(current, str +"]", 0);
  ASSERT_EQ(current, str);
  ASSERT_EQ(i, str.length() + 1);
}

TEST(IPv6, seven) {
  std::string str("ff:ff:ff:ff:ff::ff");
  std::string current;
  size_t i = http::get_ip_literal(current, str +"]", 0);
  ASSERT_EQ(current, str);
  ASSERT_EQ(i, str.length() + 1);
}

TEST(IPv6, eight) {
  std::string str("ff::ff");
  std::string current;
  size_t i = http::get_ip_literal(current, str +"]", 0);
  ASSERT_EQ(current, str);
  ASSERT_EQ(i, str.length() + 1);
}

TEST(GetHost, one) {
  std::string str("[ff::ff]/");
  std::string current;
  size_t i = http::get_host(current, str, 0);
  ASSERT_EQ(current, "ff::ff");
  ASSERT_EQ(i, 8);
}

TEST(GetHost, two) {
  std::string str("145.23.34.2/");
  std::string current;
  size_t i = http::get_host(current, str, 0);
  ASSERT_EQ(current, "145.23.34.2");
  ASSERT_EQ(i, str.length() - 1);
}

TEST(GetHost, three) {
  std::string str("145.23.34.2:8080/");
  std::string current;
  size_t i = http::get_host(current, str, 0);
  ASSERT_EQ(current, "145.23.34.2");
  ASSERT_EQ(i, 11);
}

TEST(authorityForm, one) {
  std::string str("gmorros:pass@124.23.23.32:8080");
  http::URL expected = {
  	.userinfo = "gmorros:pass",
  	.host = "124.23.23.32:8080",
  };
  http::URL current;
  http::parse_authority_form(current, str);
  ASSERT_EQ(current, expected);
}

TEST(authorityForm, two) {
  std::string str("124.23.23.32:8080");
  http::URL expected = {
	  .host = "124.23.23.32:8080",
  };
  http::URL current;
  http::parse_authority_form(current, str);
  ASSERT_EQ(current, expected);
}