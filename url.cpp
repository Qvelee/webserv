#include "url.hpp"
#include <string>
#include <sstream>
#include "error_http.hpp"
#include "utility_http.hpp"

namespace http{

namespace url {

// pct-encoded = "%" HEXDIG HEXDIG
bool isPctEncoded(std::string const &str, size_t pos) {
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

std::string decodePCT(const std::string &str) {
  std::string decoded_str;
  decoded_str.reserve(str.length());
  for (size_t i = 0; i < str.length(); ++i) {
	if (isPctEncoded(str, i)) {
	  std::stringstream ss;
	  ss << std::hex << str[i + 1] << str[i + 2];
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

// unreserved = ALPHA DIGIT - . _ ~
bool isUnreserved(char c) {
  if (isalnum(c) || c == '-' || c == '.' || c == '_' || c == '~')
	return true;
  return false;
}

// pchar = unreserved / pct-encoded / sub-delems/ ":" / "@"
size_t isPchar(std::string const &str, size_t pos) {
  char c = str[pos];
  if (isUnreserved(c) || isSubDelim(c) || c == ':' || c == '@')
	return 1;
  if (isPctEncoded(str, pos)) {
	return 3;
  }
  return 0;
}

// segment = *pchar
size_t isSegment(std::string const &str, size_t pos) {
  return isPchar(str, pos);
}

// query = *(pchar / "/" / "?")
size_t get_query(std::string &query, std::string const &str, size_t begin) {
  size_t pos = begin;
  while (pos < str.length()) {
	size_t i;
	if ((i = isPchar(str, begin)) != 0) {
	  pos += i;
	  continue;
	}
	if (str[pos] == '/' || str[pos] == '?') {
	  ++pos;
	  continue;
	}
	break;
  }
  query = str.substr(begin, pos - begin);
  return pos - begin;
}

// absolute-path = 1 *("/" segment)
size_t get_absolute_path(std::string &path, std::string const &str, size_t begin) {
  size_t pos = begin;
  while (pos < str.length() && str[pos] == '/') {
	++pos;
	size_t i;
	while ((i = isSegment(str, pos)) != 0)
	  pos += i;
  }
  path = str.substr(begin, pos - begin);
  return pos - begin;
}

// origin-form = absolute-path [ "?" query ]
void parse_origin_form(URL &url, const std::string &str) {
  size_t begin_word = 0;
  begin_word += get_absolute_path(url.raw_path, str, begin_word);
  url.path = decodePCT(url.raw_path);
  if (url.path == url.raw_path) {
	url.raw_path.clear();
  }
  if (begin_word < str.length()) {
	if (str[begin_word] == '?') {
	  ++begin_word;
	  begin_word += get_query(url.raw_query, str, begin_word);
	} else {
	  error(400);
	}
  }
  if (begin_word != str.length())
	error(400);
}

// userinfo = *(unreserved / pct-encoded / sub-delims / ":")
size_t get_userinfo(std::string &userinfo, std::string const &str, size_t begin) {
  size_t pos = begin;
  while (pos < str.length()) {
	size_t i;
	if ((i = isPctEncoded(str, pos)) != 0) {
	  pos += i;
	  continue;
	}
	char c = str[pos];
	if (isUnreserved(c) || isSubDelim(c) || c == ':') {
	  ++pos;
	  continue;
	}
	break;
  }
  userinfo = str.substr(begin, pos - begin);
  tolower(userinfo);
  return pos - begin;
}

/*
 * IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet
 * dec-octet = DIGIT ; 0-9
 * / %x31-39 DIGIT ; 10-99
 * / "1" 2DIGIT ; 100-199
 * / "2" %x30-34 DIGIT ; 200-249
 * / "25" %x30-35 ; 250-255
 */
bool is_octet(const std::string &str) {
  if (str.length() == 1) {
	if (isdigit(str[0]))
	  return true;
  } else if (str.length() == 2) {
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

bool isIPv4(const std::string &str) {
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
	  if (i >= dst.length() || dst[i] != ':')
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
 * reg-name = *(unreserved / pct-encoded / sub-delims)
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
	tolower(host);
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

// port = *DIGIT
size_t get_port(std::string &port, const std::string &str, size_t begin) {
  size_t pos = begin;
  while (isdigit(str[pos])) {
	++pos;
  }
  port = str.substr(begin, pos - begin);
  return pos - begin;
}

// authority = [userinfo "@"] host [":" port]
size_t get_authority(URL &url, std::string const &str, size_t begin) {
  size_t pos = begin;
  if (str.find('@') != std::string::npos)
	pos += get_userinfo(url.userinfo, str, pos);
  if (pos < str.length() && str[pos] == '@')
	++pos;
  pos += get_host(url.host, str, pos);
  if (pos != str.length() && str[pos] == ':') {
	++pos;
	std::string tmp;
	pos += get_port(tmp, str, pos);
	url.host += ":" + tmp;
  }
  return pos - begin;
}

// authority-form = authority
void parse_authority_form(URL &url, const std::string &str) {
  if (get_authority(url, str, 0) != str.length())
	error(400);
}

// asterisk-form = "*"
void parse_asterisk_form(URL &, const std::string &str) {
  if (str.length() != 1 || str[0] != '*')
	error(400);
}

/*
 * relative-part = "//" authority path-abempty
 * / path-absolute
 * / path-noscheme
 * / path-empty
 *
 * path-abempty  = *( "/" segment )
 * path-absolute = "/" [ segment-nz *( "/" segment ) ]
 * path-noscheme = segment-nz-nc *( "/" segment )
 * path-rootless = segment-nz *( "/" segment )
 * path-empty    = 0<pchar>
 */
size_t get_relative_part(URL &url, std::string const &str, size_t begin) {
  size_t pos = begin;
  if (str.length() < 2 || str[pos] != '/' || str[pos + 1] != '/')
	return 0;
  pos += 2;
  pos += get_authority(url, str, pos);
  if (pos < str.length()) {
	size_t i;
	if ((i = isPchar(str, pos)) != 0) {
	  pos += i;

	}
  }
  return pos - begin;
}

// partial-URI = relative-part [ "?" query ]
void parse_partial_uri(URL &url, const std::string &str) {
  size_t pos = 0;
  pos += get_relative_part(url, str, pos);
  if (pos < str.length()) {
	if (str[pos] == '?') {
	  pos++;
	  pos += get_query(url.raw_query, str, pos);
	} else {
	  error(400);
	}
  }
  if (pos != str.length())
	error(400);
}

void parse_absolute_uri(URL &, const std::string &) {

}

}
}