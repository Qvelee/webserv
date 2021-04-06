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

		while (istchar(*bytes))
			++bytes;
		if (bytes - begin_word > 15)
			error(501);
		if (*bytes != *SP)
			error(400);
		start_line_.method_.assign(begin_word, bytes - begin_word);

		++bytes;
		begin_word = bytes;

		while (*bytes != *SP && *bytes != '\0')
			++bytes;
		if (*bytes != *SP)
			error(400);
		if (bytes - begin_word > 8000)
			error(414);
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
		std::string	field_name;
		std::string field_value;

		if (*bytes == *SP)
			error(400);

		while (strncmp(CRLF, bytes, 2) != 0)
		{
			while (istchar(*bytes))
				++bytes;
			if (*bytes != ':')
				error(400);
			field_name.assign(begin_word, bytes - begin_word);
			++bytes;

			while (isblank(*bytes))
				++bytes;
			begin_word = bytes;

			do
			{
				bytes = begin_word;
				while (!(*bytes >= 0 && *bytes <= 127) || isgraph(*bytes) ||
				isblank(*bytes))
					++bytes;
				if (strncmp(CRLF, bytes, 2) != 0)
					error(400);
				field_value.append(begin_word, bytes - begin_word);
				field_value.append(SP);
				begin_word = bytes + 2;
				while (isblank(*begin_word))
					++begin_word;
			} while (isblank(*(bytes + 2)));

			bytes = begin_word;
			while (isblank(*field_value.rbegin()))
				field_value.erase(field_value.end() - 1);

			headers_[field_name].push_back(field_value);
			field_value.clear();
			field_name.clear();
		}
		bytes += 2;
	}
}