#include "http.hpp"
#include "error_http.hpp"
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
size_t get_token(std::string& dst, std::string const &data, size_t begin) {
  size_t pos = begin;
  while (istchar(data[pos]))
    ++pos;
  if (pos - begin == 0)
	error(400);
  dst.append(data, begin, pos - begin);
  return pos - begin;
}

size_t get_request_target(std::string& dst, std::string const &data, size_t begin) {
  size_t pos = begin;
  while (data[pos] != ' ' && data[pos] != '\0')
    ++pos;
  if (data[pos] != ' ')
	error(400);
  dst.append(data, begin, pos - begin);
  return pos - begin;
}

/*
 * HTTP-version = HTTP-name "/" DIGIT "." DIGIT
 * HTTP-name = %x48.54.54.50 ; "HTTP", case-sensitive
 */
size_t get_http_version(std::string& dst, std::string const &data, size_t begin) {
  size_t pos = begin;
  if (data.compare(begin, 5, "HTTP/") != 0)
	error(400);
  pos += 5;
  if (!isdigit(data[pos]))
	error(400);
  ++pos;
  if (data[pos] != '.')
	error(400);
  ++pos;
  if (!isdigit(data[pos]))
	error(400);
  ++pos;
  dst.append(data, begin, pos - begin);
  return pos - begin;
}

// HTAB = '\t'; // %x09
// SP = ' '; // %x20
// OWS = *(SP / HTAB)
// BWS = OWS
// RWS = 1*(SP/ HTAB)
// WSP = SP / HTAB ; isblank()

size_t skip_space(std::string const &data, size_t begin, SPACE space) {
  size_t pos = begin;

  switch (space) {
	case SP:
	  if (data[pos] != ' ')
		error(400);
	  ++pos;
	  break;
	case OWS:
	case BWS:
	  while (isblank(data[pos]))
	    ++pos;
	  break;
	case RWS:
	  if (!isblank(data[pos]))
		error(400);
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
size_t skip_crlf(std::string const &str, size_t begin) {
  if (str.compare(begin, 2, "\r\n") != 0)
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

size_t get_quoted_string(std::string& dst, std::string const &data, size_t begin) {
  size_t pos = begin;
  size_t begin_word = pos;

  while (isqdtext(data[pos]) || data[pos] == '\\') {
	if (data[pos] == '\\') {
	  dst.append(data, begin_word, pos - begin_word);
	  ++pos;
	  if (data[pos] == '\0' || !isquoted_pair(data[pos]))
		error(400);
	  dst.append(1, data[pos]);
	  ++pos;
	  begin_word = pos;
	} else {
	  ++pos;
	}
  }
  if (data[pos] != '"')
	error(400);
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


/*
 * URL
 */


/*
 * pct-encoded = "%" HEXDIG HEXDIG
 */
bool	isPctEncoded(std::string const &str, size_t pos) {
  if (str.length() < pos + 3)
	return false;
  if (str[pos] != '%')
	return false;
  if (!isxdigit(str[pos + 1]))
	return false;
  if (!isxdigit(str[pos + 2]))
	return false;
  return true;
}

std::string decodePCT(const std::string& str) {
  std::string decoded_str;
  decoded_str.reserve(str.length());
  for (size_t i = 0; i < str.length(); ++i) {
	if (isPctEncoded(str, i)) {
	  std::stringstream ss;
	  ss << std::hex << str[i+1] << str[i+2];
	  int num;
	  ss >> num;
	  decoded_str.push_back(num);
	  i += 2;
	} else {
	  decoded_str.push_back(str[i]);
	}
  }
  return decoded_str;
}

/*
 * reserved = gen-delim / sub-delims
 * gen = : / ? # [ ] @
 * sub = ! $ & ' ( ) * + , ; =
 *
 */
bool isGenDelim(char c) {
  if (c == ':' || c == '/' || c == '?' || c == '#' || c == '[' || c == ']' || c == '@')
	return true;
  return false;
}

bool isSubDelim(char c) {
  if (c == '!' || c == '$' || c == '&' || c == '\'' || c == '(' || c == ')' || c == '*' ||
  c == '+' || c == ',' || c == ';' || c == '=')
	return true;
  return false;
}

bool isReserved(char c) {
  if (isGenDelim(c) || isSubDelim(c))
	return true;
  return false;
}

/*
 * unreserved = ALPHA DIGIT - . _ ~
 */
bool isUnreserved(char c) {
  if (isalnum(c) || c == '-' || c == '.' || c == '_' || c == '~')
	return true;
  return false;
}

/*
 * origin-form = absolute-path [ "?" query ]
 * absolute-path = 1 *("/" segment)
 * segment = *pchar
 * pchar = unreserved / pct-encoded / sub-delems/ ":" / "@"
 *
 * query = *(pchar / "/" / "?")
 */
void parse_origin_form(URL& url, const std::string& str) {
  size_t query = str.find('?');
  url.raw_path = str.substr(0, query);
  if (url.raw_path[0] != '/')
	error(400);
  for (size_t i = 0; i < url.raw_path.length(); ++i) {
	char c = url.raw_path[i];
    if (isUnreserved(c) || isSubDelim(c) || c == ':' || c == '@' || c == '/')
	  continue;
    if (isPctEncoded(url.raw_path, i)) {
	  i += 2;
	  continue;
	}
	error(400);
  }
  url.path = decodePCT(url.raw_path);
  if (url.path == url.raw_path) {
    url.raw_path.clear();
  }
  if (query != std::string::npos) {
	url.raw_query = str.substr(query + 1);
	for (size_t i = 0; i < url.raw_query.length(); ++i) {
	  char c = url.raw_query[i];
	  if (isUnreserved(c) || isSubDelim(c) || c == ':' || c == '@' || c == '/' || c == '?')
		continue;
	  if (isPctEncoded(url.raw_query, i)) {
		i += 2;
		continue;
	  }
	  error(400);
	}
  }
}

/*
 * userinfo = *(unreserved/pct-encoded/sub-delims/":")
 */
size_t get_userinfo(std::string &dst, std::string const & src, size_t begin) {
  size_t pos = src.find('@', begin);
  if (pos != std::string::npos)
	dst = src.substr(begin, pos - begin);
  tolower(dst);
  for (size_t i = 0; i < dst.length(); ++i) {
	if (isPctEncoded(dst, i)) {
	  i += 2;
	  continue;
	}
	if (isUnreserved(dst[i]) || isSubDelim(dst[i]) || dst[i] == ':') {
	  continue;
	}
	error(400);
  }
  return dst.length() == 0 ? 0 : dst.length() + 1;
}

/*
 * IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet
 * dec-octet = DIGIT ; 0-9
 * / %x31-39 DIGIT ; 10-99
 * / "1" 2DIGIT ; 100-199
 * / "2" %x30-34 DIGIT ; 200-249
 * / "25" %x30-35 ; 250-255
 */
bool is_octet(const std::string& str) {
  if (str.length() == 1) {
    if (isdigit(str[0]))
	  return true;
  } else if (str.length() == 2) {
    if (isdigit(str[0]) && str[0] != '0' && isdigit(str[1]))
	  return true;
  } else if (str.length() == 3) {
    if (str[0] == '1') {
      if (isdigit(str[1]) && isdigit(str[2]))
		return true;
    } else if (str[0] == '2') {
      if (str[1] == '0' || str[1] == '1' || str[1] == '2' || str[1] == '3' || str[1] ==
      '4') {
        if (isdigit(str[2]))
		  return true;
      } else if (str[1] == '5') {
        if (str[2] == '0' || str[2] == '1' || str[2] == '2' || str[2] == '3' || str[2]
        == '4' || str[2] == '5')
		  return true;
      }
    }
  }
  return false;
}

bool isIPv4(const std::string& str) {
  size_t pos = 0;
  for (int i = 0; i < 4; ++i) {
    size_t end = str.find('.', pos);
    if (i != 3 && end == std::string::npos)
	  return false;
	if (!is_octet(str.substr(pos, end - pos)))
	  return false;
	pos = ++end;
  }
  return true;
}

/*
 * IP-literal = "[" (IPv6address/IPvFuture) "]"
 *
 * IPvFuture = "v" 1 * HEXDIG"."1*(unreserved/sub-delims/":")
 *
 * IPv6address =                6( h16 ":" ) ls32
 * /                       "::" 5( h16 ":" ) ls32
 * /               [ h16 ] "::" 4( h16 ":" ) ls32
 * / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
 * / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
 * / [ *3( h16 ":" ) h16 ] "::" h16 ":" ls32
 * / [ *4( h16 ":" ) h16 ] "::" ls32
 * / [ *5( h16 ":" ) h16 ] "::" h16
 * / [ *6( h16 ":" ) h16 ] "::"
 *
 * ls32 = ( h16 ":" h16 ) / IPv4address; (наименее значимый 32-битный адрес)
 * h16 = 1*4HEXDIG; (16 бит адреса представлены в шестнадцатеричном формате)
 */

size_t skip_h16(const std::string &str, size_t begin) {
  int j = 0;
  while (begin + j != str.length() && isxdigit(str[begin + j])) {
    j++;
  }
  if (j == 0 || j > 4)
	error(400);
  return j;
}

bool isLs32(const std::string &str, size_t begin) {
  if (str.find(':', begin) != std::string::npos) {
    int j = 0;
	while (begin + j != str.length() && isxdigit(str[begin + j])) {
	  j++;
	}
	if (j == 0 || j > 4 || str[j + begin] != ':')
	  return false;
	j++;
	int i = 0;
	while (begin + j + i != str.length() && isxdigit(str[begin + j + i])) {
	  i++;
	}
	if (i == 0 || i > 4)
	  return false;
	if (i + j + begin == str.length())
	  return true;
	return false;
  } else if (str.find('.', begin) != std::string::npos) {
    return isIPv4(str.substr(begin));
  }
  return false;
}

size_t get_ip_literal(std::string &dst, std::string const &str, size_t begin) {
  size_t pos = str.find(']', begin);
  if (pos == std::string::npos)
	error(400);
  dst = str.substr(begin, pos - begin);
  tolower(dst);
  if (dst[0] == 'v') {
    size_t i = 1;
    while (isxdigit(dst[i]))
      i++;
    if (i == 1)
	  error(400);
    if (dst[i++] != '.')
	  error(400);
    if (i == dst.length())
	  error(400);
	for (; i < dst.length(); i++) {
	  if (isUnreserved(dst[i]))
		continue;
	  if (isSubDelim(dst[i]) || dst[i] == ':')
		continue;
	  error(400);
	}
  } else {
    size_t i = 0;
    int beforeDoubleColon = 0;
    int afterDoubleColon = 0;
    while (dst.compare(i, 2, "::") && !isLs32(dst, i)) {
      i += skip_h16(dst, i);
      if (i >= dst.length() || dst[i] != ':' )
		error(400);
      if (!dst.compare(i, 2, "::"))
		break;
      i++;
      beforeDoubleColon++;
    }
    if (!dst.compare(i, 2, "::")) {
      i += 2;
	  if (beforeDoubleColon > 7)
		error(400);
	  while (i < dst.length()) {
	    if (isLs32(dst, i))
		  break;
	    i += skip_h16(dst, i);
	    if (i == dst.length() && afterDoubleColon == 0)
		  break;
	    if (dst[i] != ':')
		  error(400);
	    afterDoubleColon++;
	    i++;
	    if (afterDoubleColon > 6)
		  error(400);
	  }
	} else {
	  if (!isLs32(dst, i) || beforeDoubleColon != 6) {
		error(400);
	  }
    }
  }
  return dst.length() + 1;
}

/*
 * host = IP-literal / IPv4address / reg-name
 * reg-name = *( unreserved / pct-encoded / sub-delims )
 */
size_t get_host(std::string &dst, std::string const &str, size_t begin) {
  if (str[begin] == '[') {
	return get_ip_literal(dst, str, ++begin) + 1;
  } else {
    size_t slash = str.find('/', begin);
    size_t colon = str.find(':', begin);
	size_t end = colon;
    if (colon != std::string::npos) {
	  if (slash != std::string::npos)
	    end = colon < slash ? colon : slash;
    } else {
      if (slash != std::string::npos)
        end = slash;
    }
    std::string host = str.substr(begin, end - begin);
    dst = host;
    if (!isIPv4(host)) {
      for (size_t i = 0; i < dst.length(); ++i) {
        if (isUnreserved(dst[i]) || isSubDelim(dst[i]))
		  continue;
        if (isPctEncoded(dst, i)) {
          i += 2;
		  continue;
        }
		error(400);
      }
    }
	return dst.length();
  }
}

/*
 * port = *DIGIT
 */
size_t get_port(std::string &dst, const std::string &str, size_t begin) {
  size_t i = 0;
  while (isdigit(str[begin + i])) {
    i++;
  }
  dst += str.substr(begin, begin + i);
  return i;
}

/*
 * authority-form = authority
 * authority = [userinfo "@"] host [":" port]
 */
void parse_authority_form(URL& url, const std::string& str) {
  size_t end_word = 0;
  end_word += get_userinfo(url.userinfo, str, end_word);
  end_word += get_host(url.host, str, end_word);
  if (end_word != str.length() && str[end_word] == ':') {
    url.host += ":";
	get_port(url.host, str, ++end_word);
  }
}

/*
 * asterisk-form = "*"
 */
void parse_asterisk_form(URL&, const std::string& str) {
  if (str.length() != 1 || str[0] != '*')
	error(400);
}

}