#include <cctype>
#include <cstring>
#include <sstream>
#include "utility_http.hpp"

namespace http {
/*
CTL = %x00-1F / %x7F ; isctrl()
DIGIT = %x30-39 ; 0-9, isdigit()
HEXDIG = DIGIT / "A" / "B" / "C" / "D" / "E" / "F" / G" ;
				  													isxdigit()
LWSP = *(WSP / CRLF WSP);
OCTET = %x00-FF ; isprint() || iscntrl()
VCHAR = %x21-7E ; isgraph()

cписки генерировать так
1#elem => elem *(OWS "," OWS elem)
#elem => [ 1#elem ]
<n>#<m>element => element <n-1>*<m-1>( OWS "," OWS element )
принимать так
#element => [ ( "," / element ) *( OWS "," [ OWS element ] ) ]
1#element => *( "," OWS ) element *( OWS "," [ OWS element ] )


comment = "(" *( ctext / quoted-pair / comment ) ")"
ctext = HTAB / SP / %x21-27 / %x2A-5B / %x5D-7E / obs-text
*/

bool istchar(int c) {
  return (isalpha(c) || isdigit(c) || c == '!' || c == '#' || c == '$' ||
	  c == '%' || c == '&' || c == '\'' || c == '*' || c == '+' || c == '-' ||
	  c == '.' || c == '^' || c == '_' || c == '`' || c == '|' || c == '~');
}

// token = 1*tchar
size_t get_token(std::string &dst,
				 std::string const &data,
				 size_t begin,
				 StatusCode &code) {
  size_t pos = begin;
  while (istchar(data[pos]))
	++pos;
  if (pos - begin == 0) {
	code = StatusBadRequest;
	return 0;
  }
  dst.append(data, begin, pos - begin);
  return pos - begin;
}

size_t get_request_target(std::string &dst, std::string const &data, size_t begin,
						  StatusCode &code) {
  size_t pos = begin;
  while (data[pos] != ' ' && data[pos] != '\0')
	++pos;
  if (data[pos] != ' ') {
	code = StatusBadRequest;
	return 0;
  }
  dst.append(data, begin, pos - begin);
  return pos - begin;
}

/*
 * HTTP-version = HTTP-name "/" DIGIT "." DIGIT
 * HTTP-name = %x48.54.54.50 ; "HTTP", case-sensitive
 */
size_t get_http_version(std::string &dst, std::string const &data, size_t begin,
						StatusCode &code) {
  size_t pos = begin;
  if (data.compare(begin, 5, "HTTP/") != 0) {
	code = StatusHTTPVersionNotSupported;
	return 0;
  }
  pos += 5;
  if (!isdigit(data[pos])) {
	code = StatusBadRequest;
	return 0;
  }
  ++pos;
  if (data[pos] != '.') {
	code = StatusBadRequest;
	return 0;
  }
  ++pos;
  if (!isdigit(data[pos])) {
	code = StatusBadRequest;
	return 0;
  }
  ++pos;
  dst.append(data, begin, pos - begin);
  return pos - begin;
}

/*
 * HTAB = '\t'; // %x09
 * SP = ' '; // %x20
 * OWS = *(SP / HTAB)
 * BWS = OWS
 * RWS = 1*(SP/ HTAB)
 * WSP = SP / HTAB ; isblank()
 */

size_t skip_space(std::string const &data, size_t begin, SPACE space, StatusCode &code) {
  size_t pos = begin;

  switch (space) {
	case SP:
	  if (data[pos] != ' ') {
		code = StatusBadRequest;
		return 0;
	  }
	  ++pos;
	  break;
	case OWS:
	case BWS:
	  while (isblank(data[pos]))
		++pos;
	  break;
	case RWS:
	  if (!isblank(data[pos])) {
		code = StatusBadRequest;
		return 0;
	  }
	  while (isblank(data[pos]))
		++pos;
	  break;
  }
  return pos - begin;
}

/*
 * CR = "\r"; // %x0D
 * CRLF = "\r\n";
 * LF = "\n"; // %x0A
*/
size_t skip_crlf(std::string const &str, size_t begin, StatusCode &code) {
  if (str.compare(begin, 2, "\r\n") != 0) {
	code = StatusBadRequest;
	return 0;
  }
  return 2;
}

/*
 * quoted-string = DQUOTE *( qdtext / quoted-pair ) DQUOTE
 * qdtext = HTAB / SP /%x21 / %x23-5B / %x5D-7E / obs-text
 * obs-text = %x80-FF
 *
 * quoted-pair = "\" ( HTAB / SP / VCHAR / obs-text )
 */

bool isqdtext(int c) {
  return (isblank(c) || c == 0x21 || (c >= 0x23 && c <= 0x5B) ||
	  (c >= 0x5D && c <= 0x7E) || (c >= 0x80 && c <= 0xFF));
}

bool isquoted_pair(int c) {
  return (c == '\t' || c == ' ' || isgraph(c) || c > 127);
}

size_t get_quoted_string(std::string &dst, std::string const &data, size_t begin,
						 StatusCode &code) {
  size_t pos = begin;
  size_t begin_word = pos;

  while (isqdtext(data[pos]) || data[pos] == '\\') {
	if (data[pos] == '\\') {
	  dst.append(data, begin_word, pos - begin_word);
	  ++pos;
	  if (data[pos] == '\0' || !isquoted_pair(data[pos])) {
		code = StatusBadRequest;
		return 0;
	  }
	  dst.append(1, data[pos]);
	  ++pos;
	  begin_word = pos;
	} else {
	  ++pos;
	}
  }
  if (data[pos] != '"') {
	code = StatusBadRequest;
	return 0;
  }
  dst.append(data, begin_word, pos - begin_word);
  ++pos;
  return pos - begin;
}

void tolower(std::string &str) {
  std::string::iterator begin = str.begin();
  std::string::iterator end = str.end();
  for (; begin != end; ++begin)
	*begin = static_cast<char>(std::tolower(*begin));
}

}