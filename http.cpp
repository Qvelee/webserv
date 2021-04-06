#include "http.hpp"
#include <cstring>
#include "utility_http.hpp"
#include "error_http.hpp"

namespace http {
	message::message(char *bytes)
	{
		parse_start_line(bytes);
		parse_headers(bytes);
//		parse_message_body(bytes);
	}

	void message::parse_start_line(char *&bytes)
	{
		char *begin_word = bytes;

		while (istchar(*bytes)) {
			++bytes;
		}

		start_line_.method_ = str_to_method(begin_word, bytes - begin_word);
		if (*bytes != *SP)
			error(400);

		++bytes;
		begin_word = bytes;

		while (*bytes != *SP && *bytes != '\0')
			++bytes;
		if (*bytes != *SP)
			error(400);
		start_line_.request_target_.assign(begin_word, bytes - begin_word);

		++bytes;
		begin_word = bytes;
		if (strncmp(bytes, "HTTP/", 5) != 0)
			error(400);
		bytes += 5;
		if (!isdigit(*bytes))
			error(400);
		++bytes;
		if (*bytes != '.')
			error(400);
		++bytes;
		if (!isdigit(*bytes))
			error(400);
		++bytes;

		start_line_.http_version.assign(begin_word, bytes - begin_word);
		if (strncmp(bytes, "\r\n", 2) != 0)
			error(400);
		bytes += 2;
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