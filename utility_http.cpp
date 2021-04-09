#include "http.hpp"
#include "error_http.hpp"
#include <cctype>
#include <cstring>
#include "utility_http.hpp"

namespace http {

/*
 * tchar = "!" / "#" / "$" / "%" / "&" / "’" / "*"
 * / "+" / "-" / "." / "^" / "_" / "‘" / "|" / "~"
 * / DIGIT / ALPHA
 */
bool istchar(int c) {
  return (isalpha(c) || isdigit(c) || c == '!' || c == '#' || c == '$' ||
	  c == '%' || c == '&' || c == '\'' || c == '*' || c == '+' || c == '-' ||
	  c == '.' || c == '^' || c == '_' || c == '`' || c == '|' || c == '~');
}

/*
 * token = 1*tchar
 */
std::size_t get_token(std::string& dst, const char *src) {
  std::size_t size = 0;

  while (istchar(*(src + size)))
    ++size;
  if (size == 0)
	error(400);
  dst.append(src, size);

  return size;
}

std::size_t get_request_target(std::string& dst, const char *src) {
  std::size_t size = 0;

  while (*(src + size) != ' ' && *(src + size) != '\0')
    ++size;
  if (*(src + size) != ' ')
	error(400);
  dst.append(src, size);
  return size;
}

/*
 * HTTP-version = HTTP-name "/" DIGIT "." DIGIT
 * HTTP-name = %x48.54.54.50 ; "HTTP", case-sensitive
 */
std::size_t get_http_version(std::string& dst, const char *src) {
  std::size_t size = 0;

  if (strncmp(src, "HTTP/", 5) != 0)
	error(400);
  size += 5;
  if (!isdigit(*(src + size)))
	error(400);
  ++size;
  if (*(src + size) != '.')
	error(400);
  ++size;
  if (!isdigit(*(src + size)))
	error(400);
  ++size;
  dst.append(src, size);

  return size;
}

// HTAB = '\t'; // %x09
// SP = ' '; // %x20
// OWS = *(SP / HTAB)
// BWS = OWS
// RWS = 1*(SP/ HTAB)
// WSP = SP / HTAB ; isblank()

std::size_t skip_space(const char *src, SPACE space) {
  std::size_t size = 0;

  switch (space) {
	case SP:
	  if (*src != ' ')
		error(400);
	  size = 1;
	  break;
	case OWS:
	case BWS:
	  while (isblank(*(src + size)))
	    ++size;
	  break;
	case RWS:
	  if (!isblank(*src))
		error(400);
	  while (isblank(*(src + size)))
		++size;
	  break;
  }
  return size;
}

/*
 * CR = "\r"; // %x0D
 * CRLF = "\r\n";
 * LF = "\n"; // %x0A
*/
std::size_t skip_crlf(const char *src) {
  if (strncmp("\r\n", src, 2) != 0)
	error(400);
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

std::size_t get_quoted_string(std::string& dst, const char *src) {
  std::size_t size = 0;
  std::size_t begin_word = 0;

  while (isqdtext(*(src + size)) || *(src + size) == '\\') {
	if (*(src + size) == '\\') {
	  dst.append(src + begin_word, size - begin_word);
	  ++size;
	  if (*(src + size) == '\0' || !isquoted_pair(*(src + size)))
		error(400);
	  dst.append(1, *(src + size));
	  ++size;
	  begin_word = size;
	} else {
	  ++size;
	}
  }
  if (*(src + size) != '"')
	error(400);
  dst.append(src + begin_word, size - begin_word);
  ++size;

  return size;
}

void tolower(std::string &str) {
  std::string::iterator begin = str.begin();
  std::string::iterator end = str.end();
  for (; begin != end; ++begin)
    *begin = std::tolower(*begin);
}

}