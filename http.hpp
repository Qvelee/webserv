#ifndef WEBSERV_HTTP_HPP
#define WEBSERV_HTTP_HPP

#include <string>
#include <vector>
#include <map>

namespace http {

	const char* const CR = "\r";
	const char* const LF = "\n";
	const char* const HTAB = "\t";
	const char* const DQUOTE = "\"";
	const char* const SP = " ";
	const char* const CRLF = "\r\n";
//	CTL = isctrl();
//	DIGIT = isdigit();
//	HEXDIG = isxdigit();
//	VCHAR = isgraph();
//	WSP = isblank();
//	OCTET = isprint() || iscntrl();
//	LWSP = *(WSP / CRLF WSP);
//	OWS = *(SP / HTAB)

	class message
	{
	public:
		message(char *bytes);

		enum e_method
		{
			GET
		};


//	protected:
		struct request_line
		{
			e_method method_;
			std::string request_target_;
			std::string http_version;
		};

		struct status_line
		{
		};

		void parse_start_line(char *&bytes);

		void parse_headers(char *&bytes);

		void parse_message_body(char *&bytes);

//	private:
		message();

		request_line										start_line_;
		std::map<std::string, std::vector<std::string> >	headers_;
		char												*message_body_;
	};
}

#endif
