#include "../http.hpp"
#include <gtest/gtest.h>

std::ostream& operator<<(std::ostream& os, const http::message::request_line&
rl) {
	os << rl.method_ << ", ";
	os << rl.request_target_ << ", ";
	os << rl.http_version << "";
	return os;
}

TEST(TestParserStartLine, NoFail) {
	http::message::request_line expected =
			{"GET", "/me",
										 "HTTP/1.1"};
	char message[] = "GET /me HTTP/1.1\r\n\r\n";
	http::message current(message);
	ASSERT_EQ(current.start_line_, expected);
}

TEST(TestParserStartLine, SpaceBeforeMethod) {
	char message[] = " GET /me HTTP/1.1\r\n\r\n";
	ASSERT_ANY_THROW(http::message current(message));
}

TEST(TestParserStartLine, NotTcharInMethod) {
	char message[] = "G(ET /me HTTP/1.1\r\n\r\n";
	ASSERT_ANY_THROW(http::message current(message));
}

TEST(TestParserStartLine, OddSpace1) {
	char message[] = "GET  /me HTTP/1.1\r\n\r\n";
	ASSERT_ANY_THROW(http::message current(message));
}

TEST(TestParserStartLine, OddSpace2) {
	char message[] = "GET /me HTTP/1.1 \r\n\r\n";
	ASSERT_ANY_THROW(http::message current(message));
}

TEST(TestParserStartLine, OddSpace3) {
	char message[] = "GET /me  HTTP/1.1\r\n\r\n";
	ASSERT_ANY_THROW(http::message current(message));
}

TEST(TestParserStartLine, WrongProtoco1) {
	char message[] = "GET /me HTTp/1.1\r\n\r\n";
	ASSERT_ANY_THROW(http::message current(message));
}

TEST(TestParserStartLine, WrongProtoco2) {
	char message[] = "GET /me HTTP/10.1\r\n\r\n";
	ASSERT_ANY_THROW(http::message current(message));
}

TEST(TestParserStartLine, WrongProtoco3) {
	char message[] = "GET /me HTTP/1,1\r\n\r\n";
	ASSERT_ANY_THROW(http::message current(message));
}
