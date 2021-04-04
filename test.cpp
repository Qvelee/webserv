#include "http.hpp"
#include <iostream>
#include <gtest/gtest.h>

std::ostream& operator<<(std::ostream& os, const http::message::request_line
&line)
{
	if (line.method_ == http::message::GET)
		std::cout << "GET\n";
	std::cout << line.request_target_ << "\n";
	std::cout << line.http_version << "\n";
	return os;
}

std::ostream& operator<<(std::ostream& os, const std::map<std::string,
		std::vector<std::string> > &headers)
{
	std::map<std::string, std::vector<std::string> >::const_iterator begin =
			headers.begin();
	std::map<std::string, std::vector<std::string> >::const_iterator end =
			headers.end();
	for (; begin != end; ++begin)
	{
		std::cout << begin->first << ":";
		for (std::vector<std::string>::const_iterator first = begin->second.begin
				(); first != begin->second.end(); ++first)
			std::cout << *first << ", ";
		std::cout << "\n";
	}
	return os;
}

TEST(TestSuite, TestName) {
	ASSERT_EQ(1,1);
}