#include "../http.hpp"
#include <gtest/gtest.h>

std::ostream& operator<<(std::ostream& os, const http::message::request_line&
rl) {
	os << rl.request_target_ << "\n";
	os << rl.http_version << "\n";
	return os;
}

TEST(TestParserStartLine, NoFail) {
	http::message::request_line expected = {http::message::GET, "/me",
										 "HTTP/1.1"};
	char message[] = "GET /me HTTP/1.1\r\n\r\n";
	http::message current(message);

	ASSERT_EQ(current.start_line_, expected);
}

TEST(TestParserStartLine, Fail1) {
	char message[] = " GET /me HTTP/1.1\r\n\r\n";

	ASSERT_ANY_THROW(http::message current(message));
}

TEST(TestParserStartLine, Fail2) {
	char message[] = "GET  /me HTTP/1.1\r\n\r\n";

	ASSERT_ANY_THROW(http::message current(message));
}

TEST(TestParserStartLine, Fail3) {
	char message[] = "GET /me HTTP/1.1 \r\n\r\n";

	ASSERT_ANY_THROW(http::message current(message));
}