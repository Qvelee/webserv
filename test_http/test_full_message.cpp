#include "../http.hpp"
#include <gtest/gtest.h>
#include "../test_http/utility.hpp"

TEST(TestHost, one) {
  std::string message = "GET /me HTTP/1.1\r\n"
				   "Host:www.example.ru\r\n"
	   "\r\n";
  http::Request expected = {
  	.method = http::GET,
	.url = {
  	  .host = "www.example.ru",
  	  .path = "/me",
	},
	.proto = "HTTP/1.1",
	.headers = {{"host", "www.example.ru"}},
	.content_length = 0,
  };
  http::Request current;
  http::parse_request(current, message);
  ASSERT_EQ(current, expected);
}