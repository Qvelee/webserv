#include "../url.hpp"
#include "../http.hpp"
#include "gtest/gtest.h"

TEST(GetPctEncode, Simple) {
  std::string str("");
  ASSERT_EQ(http::url::isPctEncoded(str, 0), false);
}

TEST(GetPctEncode, Simple2) {
  std::string str("%");
  ASSERT_EQ(http::url::isPctEncoded(str, 0), false);
}

TEST(GetPctEncode, Simple3) {
  std::string str("ffg");
  ASSERT_EQ(http::url::isPctEncoded(str, 0), false);
}

TEST(GetPctEncode, Simple4) {
  std::string str("%fu");
  ASSERT_EQ(http::url::isPctEncoded(str, 0), false);
}

TEST(GetPctEncode, Simple5) {
  std::string str("%fe");
  ASSERT_EQ(http::url::isPctEncoded(str, 0), true);
}

TEST(decodePCT, one) {
  std::string str("Hello%20world%21%0A");
  ASSERT_EQ(http::url::decodePCT(str), "Hello world!\n");
}

TEST(originForm, one) {
  std::string str("/hello%20world/text.txt");
  http::url::URL current;
  http::url::URL expected = {
	  .path = "/hello world/text.txt",
	  .raw_path = str,
  };
  http::url::parse_origin_form(current, str);
  ASSERT_EQ(current, expected);
}

TEST(originForm, two) {
  std::string str("/text.txt");
  http::url::URL current;
  http::url::URL expected = {
	  .path = "/text.txt",
  };
  http::url::parse_origin_form(current, str);
  ASSERT_EQ(current, expected);
}

TEST(originForm, three) {
  std::string str("/he[lo%20world/text.txt");
  http::url::URL current;
  ASSERT_ANY_THROW(http::url::parse_origin_form(current, str));
}

TEST(originForm, four) {
  std::string str("/text.txt?param1=one&param2=two");
  http::url::URL current;
  http::url::URL expected = {
	  .path = "/text.txt",
	  .raw_query = "param1=one&param2=two",
  };
  http::url::parse_origin_form(current, str);
  ASSERT_EQ(current, expected);
}

TEST(GetUserInfo, one) {
  std::string str("gmorros:password");
  std::string current;
  size_t i = http::url::get_userinfo(current, str + "@", 0);
  ASSERT_EQ(current, str);
  ASSERT_EQ(i, str.length());
}

TEST(GetUserInfo, two) {
  std::string str("");
  std::string current;
  size_t i = http::url::get_userinfo(current, str, 0);
  ASSERT_EQ(current, str);
  ASSERT_EQ(i, 0);
}

TEST(GetUserInfo, three) {
  std::string str("[gmorros@");
  std::string current;
  size_t i = http::url::get_userinfo(current, str, 0);
  ASSERT_EQ(current, "");
  ASSERT_EQ(i, 0);
}

TEST(IPv4, one) {
  ASSERT_EQ(http::url::isIPv4("255.2.134.11"), true);
}

TEST(IPv4, two) {
  ASSERT_EQ(http::url::isIPv4("255.2.134.234"), true);
}

TEST(IPv4, three) {
  ASSERT_EQ(http::url::isIPv4("255.2.134.269"), false);
}

TEST(IPv4, four) {
  ASSERT_EQ(http::url::isIPv4("259.2.134.200"), false);
}

TEST(IpLiteral, future1) {
  std::string str("vf5e4.sgds");
  std::string current;
  size_t i = http::url::get_ip_literal(current, str + "]", 0);
  ASSERT_EQ(current, str);
  ASSERT_EQ(i, 11);
}

TEST(IpLiteral, future2) {
  std::string str("f5e4.sgds");
  std::string current;
  ASSERT_ANY_THROW(http::url::get_ip_literal(current, str + "]", 0));
}

TEST(IpLiteral, future3) {
  std::string str("v.sgds");
  std::string current;
  ASSERT_ANY_THROW(http::url::get_ip_literal(current, str + "]", 0));
}

TEST(IpLiteral, future4) {
  std::string str("vfe");
  std::string current;
  ASSERT_ANY_THROW(http::url::get_ip_literal(current, str + "]", 0));
}

TEST(IpLiteral, future5) {
  std::string str("vfe.");
  std::string current;
  ASSERT_ANY_THROW(http::url::get_ip_literal(current, str + "]", 0));
}

TEST(IPv6, one) {
  std::string str("ff:ee:ff:ff:ff:ff:ff:ff");
  std::string current;
  size_t i = http::url::get_ip_literal(current, str +"]", 0);
  ASSERT_EQ(current, str);
  ASSERT_EQ(i, str.length() + 1);
}

TEST(IPv6, two) {
  std::string str("::ee:ff:ff:ff:ff:ff:ff");
  std::string current;
  size_t i = http::url::get_ip_literal(current, str +"]", 0);
  ASSERT_EQ(current, str);
  ASSERT_EQ(i, str.length() + 1);
}

TEST(IPv6, three) {
  std::string str("::ff:ff:ff:ff:ff:ff");
  std::string current;
  size_t i = http::url::get_ip_literal(current, str +"]", 0);
  ASSERT_EQ(current, str);
  ASSERT_EQ(i, str.length() + 1);
}

TEST(IPv6, four) {
  std::string str("::ff:ff:ff:255.244.244.7");
  std::string current;
  size_t i = http::url::get_ip_literal(current, str +"]", 0);
  ASSERT_EQ(current, str);
  ASSERT_EQ(i, str.length() + 1);
}

TEST(IPv6, fife) {
  std::string str("ff:ff::ff:ff:ff:255.244.244.7");
  std::string current;
  size_t i = http::url::get_ip_literal(current, str +"]", 0);
  ASSERT_EQ(current, str);
  ASSERT_EQ(i, str.length() + 1);
}

TEST(IPv6, six) {
  std::string str("ff:ff:ff:ff:ff::");
  std::string current;
  size_t i = http::url::get_ip_literal(current, str +"]", 0);
  ASSERT_EQ(current, str);
  ASSERT_EQ(i, str.length() + 1);
}

TEST(IPv6, seven) {
  std::string str("ff:ff:ff:ff:ff::ff");
  std::string current;
  size_t i = http::url::get_ip_literal(current, str +"]", 0);
  ASSERT_EQ(current, str);
  ASSERT_EQ(i, str.length() + 1);
}

TEST(IPv6, eight) {
  std::string str("ff::ff");
  std::string current;
  size_t i = http::url::get_ip_literal(current, str +"]", 0);
  ASSERT_EQ(current, str);
  ASSERT_EQ(i, str.length() + 1);
}

TEST(GetHost, one) {
  std::string str("[ff::ff]/");
  std::string current;
  size_t i = http::url::get_host(current, str, 0);
  ASSERT_EQ(current, "ff::ff");
  ASSERT_EQ(i, 8);
}

TEST(GetHost, two) {
  std::string str("145.23.34.2/");
  std::string current;
  size_t i = http::url::get_host(current, str, 0);
  ASSERT_EQ(current, "145.23.34.2");
  ASSERT_EQ(i, str.length() - 1);
}

TEST(GetHost, three) {
  std::string str("145.23.34.2:8080/");
  std::string current;
  size_t i = http::url::get_host(current, str, 0);
  ASSERT_EQ(current, "145.23.34.2");
  ASSERT_EQ(i, 11);
}

TEST(authorityForm, one) {
  std::string str("gmorros:pass@124.23.23.32:8080");
  http::url::URL expected = {
	  .userinfo = "gmorros:pass",
	  .host = "124.23.23.32:8080",
  };
  http::url::URL current;
  http::url::parse_authority_form(current, str);
  ASSERT_EQ(current, expected);
}

TEST(authorityForm, two) {
  std::string str("124.23.23.32:8080");
  http::url::URL expected = {
	  .host = "124.23.23.32:8080",
  };
  http::url::URL current;
  http::url::parse_authority_form(current, str);
  ASSERT_EQ(current, expected);
}
