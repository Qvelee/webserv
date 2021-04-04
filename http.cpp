#include "http.hpp"
#include <cstring>

namespace http
{
	void error(int code) {
		throw code;
	}

	message::e_method str_to_method(char *, size_t n)
	{
		if (n > 5)
			error(501);
		message::e_method method = message::GET;
		return method;
	}

	message::message(char *bytes)
	{
		parse_start_line(bytes);
		parse_headers(bytes);
//		parse_message_body(bytes);
	}

	void message::parse_start_line(char *&bytes)
	{
		char *begin_word = bytes;
		char *end_word = bytes;

		bytes = strstr(bytes, CRLF);
		if (bytes == NULL)
			error(400);

		end_word = strchr(begin_word, *SP);
		if (end_word == NULL || end_word >= bytes)
			error(400);
		start_line_.method_ = str_to_method(begin_word, end_word - begin_word);
		begin_word = end_word + 1;

		end_word = strchr(begin_word, *SP);
		if (end_word == NULL || end_word >= bytes)
			error(400);
		if (end_word - begin_word > 8000 || end_word - begin_word == 0)
			error(414);
		start_line_.request_target_.assign(begin_word, end_word - begin_word);
		begin_word = end_word + 1;

		if (static_cast<size_t>(bytes - begin_word) != strlen("HTTP/x.x"))
			error(400);
		start_line_.http_version.assign(begin_word, bytes - begin_word);

		bytes += strlen(CRLF);
	}

	void message::parse_headers(char *&bytes)
	{
		char		*begin_word = bytes;
		char		*end_word = bytes;
		std::string	field_name;
		std::string field_value;

		if (*bytes == ' ')
			error(400);

		while (strncmp(CRLF, begin_word, 3) != 0)
		{
			end_word = strchr(begin_word, ':');
			if (end_word == NULL)
				error(400);
			if (isblank(*(end_word - 1)))
				error(400);
			field_name.assign(begin_word, end_word - begin_word);

			begin_word = end_word + 1;
			while (isblank(*begin_word))
				++begin_word;
			do
			{
				end_word = strstr(begin_word, CRLF);
				if (end_word == NULL)
					error(400);
				field_value.append(begin_word, end_word - begin_word);
				begin_word = end_word + 2;
				while (isblank(*begin_word))
					++begin_word;
			} while (isblank(*(end_word + 2)));

			begin_word = end_word + 2;
			while (isblank(*field_value.rbegin()))
				field_value.erase(field_value.end() - 1);

			headers_[field_name].push_back(field_value);
			field_value.clear();
			field_name.clear();
		}
		bytes = begin_word;
	}
}