#include "url.hpp"
#include <string>
#include <sstream>
#include "utility_http.hpp"

namespace http{

namespace url {

// pct-encoded = "%" HEXDIG HEXDIG
bool isPctEncoded(std::string const &str, size_t pos) {
  if (str.length() < pos + 3 || str[pos] != '%' ||
  !isxdigit(str[pos+1]) || !isxdigit(str[pos+2]))
	return false;
  return true;
}

std::string decodePCT(std::string const &str) {
  std::string decoded_str;
  int c;

  decoded_str.reserve(str.length());
  for (size_t i = 0; i < str.length(); ++i) {
	if (isPctEncoded(str, i)) {
	  std::stringstream ss;
	  ss << std::hex << str[i + 1] << str[i + 2];
	  ss >> c;
	  decoded_str.push_back(c);
	  i += 2;
	} else {
	  decoded_str.push_back(str[i]);
	}
  }
  return decoded_str;
}

bool isGenDelim(char c) {
  return(c == ':' || c == '/' || c == '?' || c == '#' || c == '[' || c == ']' || c == '@');
}

bool isSubDelim(char c) {
  return(c == '!' || c == '$' || c == '&' || c == '\'' || c == '(' || c == ')' ||
  c == '*' || c == '+' || c == ',' || c == ';' || c == '=');
}

// unreserved = ALPHA DIGIT - . _ ~
bool isUnreserved(char c) {
  return(isalnum(c) || c == '-' || c == '.' || c == '_' || c == '~');
}

//scheme = ALPHA *( ALPHA / DIGIT / «+» / «-» / «.» )
ssize_t get_scheme(std::string &scheme, std::string const &str, size_t begin) {
  size_t pos = begin;
  if (pos >= str.length() || !isalpha(str[pos])) {
    return -1;
  }
  ++pos;
  while (pos < str.length()) {
	char c = str[pos];
	if (isalnum(c) || c == '+' || c == '-' || c == '.') {
	  ++pos;
	  continue;
	}
	break;
  }
  scheme = str.substr(begin, pos - begin);
  tolower(scheme);
  return static_cast<ssize_t>(pos - begin);
}

// userinfo = *(unreserved / pct-encoded / sub-delims / ":")
size_t get_userinfo(std::string &userinfo, std::string const &str, size_t begin) {
  size_t pos = begin;
  size_t i;

  while (pos < str.length()) {
	char c = str[pos];
	if (isUnreserved(c) || isSubDelim(c) || c == ':') {
	  ++pos;
	  continue;
	}
	if ((i = isPctEncoded(str, pos)) != 0) {
	  pos += i;
	  continue;
	}
	break;
  }
  userinfo = str.substr(begin, pos - begin);
  return pos - begin;
}

// h16 = 1*4HEXDIG; (16 бит адреса представлены в шестнадцатеричном формате)
ssize_t skip_h16(const std::string &str, size_t begin) {
  int j = 0;
  while (begin + j != str.length() && isxdigit(str[begin + j])) {
	j++;
  }
  if (j == 0 || j > 4) {
	return -1;
  }
  return j;
}

// ls32 = ( h16 ":" h16 ) / IPv4address; (наименее значимый 32-битный адрес)
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

/*
 * IPv6address =                6( h16 ":" ) ls32
 * /                       "::" 5( h16 ":" ) ls32
 * /               [ h16 ] "::" 4( h16 ":" ) ls32
 * / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
 * / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
 * / [ *3( h16 ":" ) h16 ] "::" h16 ":" ls32
 * / [ *4( h16 ":" ) h16 ] "::" ls32
 * / [ *5( h16 ":" ) h16 ] "::" h16
 * / [ *6( h16 ":" ) h16 ] "::"
 */
bool isIPv6(std::string const &host) {
  size_t i = 0;
  ssize_t j;
  int beforeDoubleColon = 0;
  int afterDoubleColon = 0;
  while (host.compare(i, 2, "::") && !isLs32(host, i)) {
    j = skip_h16(host, i);
    if (j == -1)
	  return false;
	i += j;
	if (i >= host.length() || host[i] != ':')
	  return false;
	if (!host.compare(i, 2, "::"))
	  break;
	i++;
	beforeDoubleColon++;
  }
  if (!host.compare(i, 2, "::")) {
	i += 2;
	if (beforeDoubleColon > 7)
	  return false;
	while (i < host.length()) {
	  if (isLs32(host, i)) {
	    if (afterDoubleColon > 5)
		  return false;
		return true;
	  }
	  j = skip_h16(host, i);
	  if (j == -1)
		return false;
	  i += j;
	  if (i == host.length())
		break;
	  if (host[i] != ':')
		return false;
	  afterDoubleColon++;
	  i++;
	  if (afterDoubleColon > 6)
		return false;
	}
	if (afterDoubleColon == 0)
	  return true;
	return false;
  } else {
	if (beforeDoubleColon != 6)
	  return false;
	return true;
  }
}

// IPvFuture = "v" 1 * HEXDIG"."1*(unreserved/sub-delims/":")
bool isIPvFuture(std::string const &host) {
  if (host.length() != 0 && host[0] == 'v') {
	size_t i = 1;
	while (isxdigit(host[i]))
	  i++;
	if (i == 1)
	  return false;
	if (host[i++] != '.')
	  return false;
	if (i == host.length())
	  return false;
	while (i < host.length()) {
	  if (isUnreserved(host[i]) || isSubDelim(host[i]) || host[i] == ':') {
		i++;
		continue;
	  }
	  return false;
	}
	return true;
  }
  return false;
}

// IP-literal = (IPv6address/IPvFuture)
bool isIPliteral(std::string const &host) {
  if (host.length() != 0 && host[0] == 'v') {
    return(isIPvFuture(host));
  } else {
	return (isIPv6(host));
  }
}

/*
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

// IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet
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

// reg-name = *(unreserved / pct-encoded / sub-delims)
bool isRegName(std::string const &host) {
  for (size_t i = 0; i < host.length(); ++i) {
	if (isUnreserved(host[i]) || isSubDelim(host[i]))
	  continue;
	if (isPctEncoded(host, i)) {
	  i += 2;
	  continue;
	}
	return false;
  }
  return true;
}

// host = IP-literal / IPv4address / reg-name
ssize_t get_host(std::string &dst, std::string const &str, size_t begin) {
  if (str[begin] == '[') {
	++begin;
	size_t end = str.find(']', begin);
	if (end == std::string::npos) {
	  return -1;
	}
	dst = str.substr(begin, end - begin);
	tolower(dst);
	if (!isIPliteral(dst)) {
	  return -1;
	}
	return static_cast<ssize_t>(dst.length()) + 2;
  } else {
    size_t end = str.find_first_of(":/?#", begin);
    if (end != std::string::npos)
	  dst = str.substr(begin, end - begin);
	else
	  dst = str.substr(begin);
	tolower(dst);
	if (!isIPv4(dst) && !isRegName(dst)) {
	  return -1;
	}
	decodePCT(dst);
	return static_cast<ssize_t>(dst.length());
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
ssize_t get_authority(URL &url, std::string const &str, size_t begin) {
  size_t pos = begin;
  ssize_t size;

  if (str.find('@') != std::string::npos) {
    pos += get_userinfo(url.userinfo, str, pos);
	if (str[pos] == '@')
	  ++pos;
	else
	  return -1;
  }
  size = get_host(url.host, str, pos);
  if (size == -1)
	return -1;
  pos += size;
  if (pos != str.length() && str[pos] == ':') {
	++pos;
	std::string tmp;
	pos += get_port(tmp, str, pos);
	url.host += ":" + tmp;
  }
  return static_cast<ssize_t>(pos - begin);
}

// pchar = unreserved / pct-encoded / sub-delims/ ":" / "@"
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

//segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
size_t isSegmentNzNc(std::string const &str, size_t pos) {
  if (str[pos] == ':')
	return 0;
  return (isPchar(str, pos));
}

// path-abempty = *("/" segment)
size_t get_abempty_path(std::string &path, std::string const &str, size_t begin) {
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

// path-absolute = "/" [ segment-nz *( "/" segment ) ]
ssize_t get_absolute_path(std::string &path, std::string const &str, size_t begin) {
  if (begin >= str.length() || str[begin] != '/')
	return -1;
  if (isSegment(str, begin + 1) == 0)
	return -1;
  return static_cast<ssize_t>(get_abempty_path(path, str, begin));
}

// path-noscheme = segment-nz-nc *( "/" segment )
ssize_t get_noscheme_path(std::string &path, std::string const &str, size_t begin) {
  if (begin >= str.length() || isSegmentNzNc(str, begin) == 0)
	return -1;
  return static_cast<ssize_t>(get_abempty_path(path, str, begin));
}

// path-rootless = segment-nz *( "/" segment )
ssize_t get_rootless_path(std::string &path, std::string const &str, size_t begin) {
  if (begin >= str.length() || isSegment(str, begin) == 0)
	return -1;
  return static_cast<ssize_t>(get_abempty_path(path, str, begin));
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

/*
 * relative-part = "//" authority path-abempty
 * / path-absolute
 * / path-noscheme
 * / path-empty
 */
ssize_t get_relative_part(URL &url, std::string const &str, size_t begin) {
  size_t pos = begin;
  ssize_t size;

  if (str.length() < 2 || str[pos] != '/' || str[pos + 1] != '/')
	return -1;
  pos += 2;
  size = get_authority(url, str, pos);
  if (size == -1)
	return -1;
  pos += size;
  if (pos < str.length()) {
	if (str[pos] == '/') {
	  pos += get_abempty_path(url.path, str, pos);
	} else {
	  size = get_noscheme_path(url.path, str, pos);
	  if (size == -1)
		return -1;
	  pos += size;
	}
  }
  return static_cast<ssize_t>(pos - begin);
}

// partial-URI = relative-part [ "?" query ]
bool parse_partial_uri(URL &url, const std::string &str) {
  ssize_t size = 0;

  size = get_relative_part(url, str, size);
  if (size == -1)
	return false;
  if (static_cast<size_t>(size) < str.length() && str[size] == '?') {
	++size;
	size += static_cast<ssize_t>(get_query(url.raw_query, str, size));
  }
  if (static_cast<size_t>(size) != str.length())
	return false;
  return true;
}

/*
 * hier-part = "//" authority path-abempty
 * / path-absolute
 * / path-rootless
 * / path-empty
*/
ssize_t get_hier_part(URL &url, std::string const &str, size_t begin) {
  size_t pos = begin;
  ssize_t size;

  if (str.length() < 2 || str[pos] != '/' || str[pos + 1] != '/')
	return -1;
  pos += 2;
  size = get_authority(url, str, pos);
  if (size == -1)
	return -1;
  pos += size;
  if (pos < str.length()) {
	if (str[pos] == '/') {
	  pos += get_abempty_path(url.path, str, pos);
	} else {
	  size = get_rootless_path(url.path, str, pos);
	  if (size == -1)
		return -1;
	  pos += size;
	}
  }
  return static_cast<ssize_t>(pos - begin);
}

// absolute-URI = scheme ":" hier-part [ "?" query ]
bool parse_absolute_uri(URL &url, const std::string &str) {
  size_t pos = 0;
  ssize_t size;

  size = get_scheme(url.scheme, str, pos);
  if (size == -1)
	return -1;
  pos += size;
  if (pos < str.length() && str[pos] == ':') {
	size = get_hier_part(url, str, pos);
	if (size == -1)
	  return false;
	pos += size;
  } else {
	return false;
  }
  if (pos < str.length() && str[pos] == '?') {
	++pos;
	pos += get_query(url.raw_query, str, pos);
  }
  if (pos != str.length())
	return false;
  return true;
}

// origin-form = absolute-path [ "?" query ]
// absolute-path = 1*( "/" segment )
bool parse_origin_form(URL &url, const std::string &str) {
  size_t begin_word = 0;
  begin_word += get_abempty_path(url.raw_path, str, begin_word);
  if (begin_word == 0)
	return false;
  url.path = decodePCT(url.raw_path);
  if (url.path == url.raw_path) {
	url.raw_path.clear();
  }
  if (begin_word < str.length() && str[begin_word] == '?') {
    ++begin_word;
    begin_word += get_query(url.raw_query, str, begin_word);
  }
  if (begin_word != str.length())
	return false;
  return true;
}

// authority-form = authority
bool parse_authority_form(URL &url, const std::string &str) {
  ssize_t size = get_authority(url, str, 0);
  if (size == -1 || static_cast<size_t>(size) != str.length())
	return false;
  return true;
}

// asterisk-form = "*"
bool parse_asterisk_form(URL &, const std::string &str) {
  if (str.length() != 1 || str[0] != '*')
	return false;
  return true;
}

}
}