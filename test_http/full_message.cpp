#include "../http.hpp"
#include <gtest/gtest.h>

TEST(TestHost, one) {
  char message[] = "GET /me HTTP/1.1\r\n"
				   "Host:www.example.ru\r\n"
	   "\r\n";
  http::Request expected = {
  	http::GET,
	{
  	  .host = "www.example.ru",
  	  .path = "/me",
	},
	"HTTP/1.1",
	{{"host", "www.example.ru"}},
	0,
  };
  http::Request current;
  http::parse_request(current, message);
  ASSERT_EQ(current, expected);
}