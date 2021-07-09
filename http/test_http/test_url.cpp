#include "../url.hpp"
#include "../test_http/utility.hpp"
#include "gtest/gtest.h"

TEST(decodePCT, one) {
  std::string str("Hello%20world%21%0A");
  ASSERT_EQ(http::url::decodePCT(str), "Hello world!\n");
}

TEST(GetScheme, one) {
  std::string current;
  http::url::get_scheme(current, "http://www.example.com", 0);
  ASSERT_EQ(current, "http");
}

TEST(GetScheme, two) {
  std::string current;
  http::url::get_scheme(current, "hTtp+://www.example.com", 0);
  ASSERT_EQ(current, "http+");
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

TEST(IpLiteral, future1) {
  std::string str("vf5e4.sgds");
  ASSERT_EQ(http::url::isIPliteral(str), true);
}

TEST(IpLiteral, future2) {
  std::string str("f5e4.sgds");
  ASSERT_EQ(http::url::isIPliteral(str), false);
}

TEST(IpLiteral, future3) {
  std::string str("v.sgds");
  ASSERT_EQ(http::url::isIPliteral(str), false);
}

TEST(IpLiteral, future4) {
  std::string str("vfe");
  ASSERT_EQ(http::url::isIPliteral(str), false);
}

TEST(IpLiteral, future5) {
  std::string str("vfe.");
  ASSERT_EQ(http::url::isIPliteral(str), false);
}

TEST(IPv6, one) {
  std::string str("ff:ee:ff:ff:ff:ff:ff:ff");
  ASSERT_EQ(http::url::isIPliteral(str), true);
}

TEST(IPv6, two) {
  std::string str("::ee:ff:ff:ff:ff:ff:ff");
  ASSERT_EQ(http::url::isIPliteral(str), true);
}

TEST(IPv6, three) {
  std::string str("::ff:ff:ff:ff:ff:ff");
  ASSERT_EQ(http::url::isIPliteral(str), true);
}

TEST(IPv6, four) {
  std::string str("::ff:ff:ff:255.244.244.7");
  ASSERT_EQ(http::url::isIPliteral(str), true);
}

TEST(IPv6, fife) {
  std::string str("ff:ff::ff:ff:ff:255.244.244.7");
  ASSERT_EQ(http::url::isIPliteral(str), true);
}

TEST(IPv6, six) {
  std::string str("ff:ff:ff:ff:ff::");
  ASSERT_EQ(http::url::isIPliteral(str), true);
}

TEST(IPv6, seven) {
  std::string str("ff:ff:ff:ff:ff::ff");
  ASSERT_EQ(http::url::isIPliteral(str), true);
}

TEST(IPv6, eight) {
  std::string str("ff::ff");
  ASSERT_EQ(http::url::isIPliteral(str), true);
}

TEST(IPv6, fail1) {
  std::string str("ff:ff");
  ASSERT_EQ(http::url::isIPliteral(str), false);
}

TEST(IPv6, fail2) {
  std::string str("ff:ff:ff:ff:ff:ff:ff:ff:ff");
  ASSERT_EQ(http::url::isIPliteral(str), false);
}

TEST(IPv6, fail3) {
  std::string str("::ff:ff:ff:ff:ff:dsf");
  ASSERT_EQ(http::url::isIPliteral(str), false);
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

TEST(GetHost, one) {
  std::string str("[ff::ff]/");
  std::string current;
  ssize_t i = http::url::get_host(current, str, 0);
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

TEST(GetHost, four) {
  std::string str("www.example@d/");
  std::string current;
  size_t i = http::url::get_host(current, str, 0);
  ASSERT_EQ(i, -1);
}

TEST(GetAuthority, one) {
  std::string str("gmorros:pass@124.23.23.32:8080");
  http::url::URL expected = {
	  .userinfo = "gmorros:pass",
	  .host = "124.23.23.32:8080",
  };
  http::url::URL current;
  ASSERT_EQ(http::url::get_authority(current, str, 0), str.length());
  ASSERT_EQ(current, expected);
}

TEST(GetAuthority, two) {
  std::string str("124.23.23.32:8080");
  http::url::URL expected = {
	  .host = "124.23.23.32:8080",
  };
  http::url::URL current;
  ASSERT_EQ(http::url::get_authority(current, str, 0), str.length());
  ASSERT_EQ(current, expected);
}

TEST(originForm, one) {
  std::string str("/hello%20world/text.txt");
  http::url::URL current;
  http::url::URL expected = {
	  .path = "/hello world/text.txt",
	  .raw_path = str,
  };
  ASSERT_EQ(http::url::parse_origin_form(current, str), true);
  ASSERT_EQ(current, expected);
}

TEST(originForm, two) {
  std::string str("/text.txt");
  http::url::URL current;
  http::url::URL expected = {
	  .path = "/text.txt",
  };
  ASSERT_EQ(http::url::parse_origin_form(current, str), true);
  ASSERT_EQ(current, expected);
}

TEST(originForm, three) {
  std::string str("/he[lo%20world/text.txt");
  http::url::URL current;
  ASSERT_EQ(http::url::parse_origin_form(current, str), false);
}

TEST(originForm, four) {
  std::string str("/text.txt?param1=one&param2=two");
  http::url::URL current;
  http::url::URL expected = {
	  .path = "/text.txt",
	  .raw_query = "param1=one&param2=two",
  };
  ASSERT_EQ(http::url::parse_origin_form(current, str), true);
  ASSERT_EQ(current, expected);
}

TEST(GetPath, abempty) {
  std::string str("/text.txt?param1=one&param2=two");
  std::string current;
  ASSERT_EQ(http::url::get_abempty_path(current, str, 0), 9);
  ASSERT_EQ(current, "/text.txt");
}

TEST(GetPath, absolute) {
  std::string str("?param1=one&param2=two");
  std::string current;
  ASSERT_EQ(http::url::get_absolute_path(current, str, 0), -1);
}
