#include "http.hpp"
#include <cstring>
#include "utility_http.hpp"
#include "error_http.hpp"
#include <sstream>

namespace http {

const char	DQUOTE = '\"';

HeaderHandlers& get_header_field_handlers() {
  static HeaderHandlers header_field_handlers = {
	  {"content-length", &content_length},
	  {"transfer-encoding", &transfer_encoding},
	  {"host", &host},
  };
  return header_field_handlers;
}

TransferCodingRegister& get_transfer_coding_register() {
  static const TransferCodingRegister transfer_coding_register = {
	  {"chunked", 1},
//	  {"compress", 1},
//	  {"deflate", 1},
//	  {"gzip", 1},
//	  {"x-compress", 1},
//	  {"x-gzip", 1},
  };
  return transfer_coding_register;
}

/*
 * HTTP-message = start-line *( header-field CRLF ) CRLF [ message-body ]
 */
bool	parse_request(Request& req, std::string const &data) {
  size_t pos = 0;
  pos += parse_request_line(req, data, pos);
  pos += parse_headers(req.headers, data, pos);
  header_analysis(req, req.headers);
  calculate_length_message(req);
  if (req.content_length != 0)
    return read_body(req, data, pos);
  return true;
}

void	parse_request_target(URL &url, Method &method, const std::string &str) {
  if (str.length() > 8000)
    error(414);
  if (method == OPTIONS) {
    parse_asterisk_form(url, str);
  } else if (method == CONNECT) {
    parse_authority_form(url, str);
  } else {
    parse_origin_form(url, str);
  }
}

void	parse_and_validate_method(Method &m, const std::string &str) {
  static std::map<const std::string, Method> methods = {
	  {"GET", GET},
	  {"HEAD", HEAD},
	  {"POST", POST},
	  {"PUT", PUT},
	  {"DELETE", DELETE},
	  {"CONNECT", CONNECT},
	  {"OPTIONS", OPTIONS},
	  {"TRACE", TRACE},
  };
  if (methods.count(str)) {
    m = methods[str];
  } else
  {
	error(400);
  }
}

/*
 * request-line = method SP request-target SP HTTP-version CRLF
 */
size_t parse_request_line(Request &r, std::string const &data, size_t begin) {
  size_t pos = begin;
  //skip first CRLF (3.5)
  if (data.compare(pos, 2, "\r\n") == 0)
    pos += skip_crlf(data, pos);
  std::string method;
  pos += get_token(method, data, pos);
  parse_and_validate_method(r.method, method);
  pos += skip_space(data, pos, SP);
  std::string request_target;
  pos += get_request_target(request_target, data, pos);
  parse_request_target(r.url, r.method, request_target);
  pos += skip_space(data, pos, SP);
  pos += get_http_version(r.proto, data, pos);
  if (r.proto != "HTTP/1.1")
	error(505);
  pos += skip_crlf(data, pos);
  return pos - begin;
}

/*
 * header-field = field-name ":" OWS field-value OWS
 */
size_t parse_headers(Headers &dst, std::string const &data, size_t begin) {
  size_t pos = begin;
  std::string field_name;
  std::string field_value;

  if (data[pos] == SP)
	error(400);

  while (data.compare(pos, 2, "\r\n") != 0) {
    // field-name = token
	pos += get_token(field_name, data, pos);
	tolower(field_name);

	if (data[pos] != ':')
	  error(400);
	++pos;

	pos += skip_space(data, pos, OWS);

	// field-value = *( field-content / obs-fold )
	// field-content = field-vchar [ 1*( SP / HTAB ) field-vchar ]
	// field-vchar = VCHAR / obs-text

	// obs-fold = CRLF 1*( SP / HTAB )
	bool obs_fold = false;
	do {
	  size_t local_size = 0;
	  while (data[pos + local_size] < 0 || isgraph(data[pos + local_size])
	   || isblank(data[pos + local_size]))
		++local_size;
	  if (data.compare(pos+local_size, 2, "\r\n") != 0)
		error(400);
	  field_value.append(data, pos, local_size);
	  field_value.append(1, ' ');
	  pos += local_size;
	  pos += 2;
	  if (isblank(data[pos]))
	    obs_fold = true;
	  else
	    obs_fold = false;
	  while (isblank(data[pos]))
	    ++pos;
	} while (obs_fold);

	// OWS
	while (isblank(*field_value.rbegin()))
	  field_value.erase(field_value.end() - 1);

	if (dst.count(field_name) == 1) {
	  if (field_name == "host")
		error(400);
	  dst[field_name].append(",");
	}
	dst[field_name].append(field_value);
	field_value.clear();
	field_name.clear();
  }
  pos += 2;
  return pos - begin;
}

void header_analysis(Request &req, Headers &h) {
  Headers::const_iterator first;
  Headers::const_iterator last;
  first = h.begin();
  last = h.end();

  HeaderHandlers &handlers = get_header_field_handlers();
  for (; first != last; ++first) {
	if (handlers.count(first->first))
	  handlers.at(first->first)(req, first->second);
  }
}

/*
 * Content-Length = 1*DIGIT
 */
void content_length(Request& req, std::string const &value) {
  if (value.find_first_not_of("0123456789") != std::string::npos)
	error(400);
  std::stringstream ss(value);
  ss >> req.content_length;
  if (ss.fail())
	error(400);
  if (req.content_length < 0)
	error(400);
}

void validate_transfer_coding(const std::string& name) {
  TransferCodingRegister& r = get_transfer_coding_register();
  if (r.count(name) == 0)
	error(501);
}

/*
 * transfer-extension = token *( OWS ";" OWS transfer-parameter )
 */
void transfer_encoding(Request& req, std::string const &extension) {
  size_t begin_word = 0;
  size_t end_word = 0;
  transfer_extension tmp_ext;
  transfer_parameter tmp_par;

  while (begin_word != extension.size()) {
	end_word += get_token(tmp_ext.token, extension, begin_word);
	tolower(tmp_ext.token);
	validate_transfer_coding(tmp_ext.token);

	end_word += skip_space(extension, end_word, OWS);

	while (extension[end_word] == ';') {
	  ++end_word;

	  end_word += skip_space(extension, end_word, OWS);
	  /*
	   * transfer-parameter = token BWS "=" BWS ( token / quoted-string )
	   */
	  begin_word = end_word;
	  end_word += get_token(tmp_par.name, extension, begin_word);

	  end_word += skip_space(extension, end_word, BWS);

	  if (extension[end_word] != '=')
		error(400);
	  ++end_word;

	  end_word += skip_space(extension, end_word, BWS);

	  begin_word = end_word;
	  //quoted-string / token
	  if (extension[end_word] == DQUOTE) {
	    ++end_word;
		++begin_word;
		end_word += get_quoted_string(tmp_par.value,
								extension, begin_word);
	  } else {
	    end_word += get_token(tmp_par.value, extension, begin_word);
	  }
	  tmp_ext.transfer_parameter.push_back(tmp_par);
	  tmp_par.name.clear();
	  tmp_par.value.clear();
	  end_word += skip_space(extension, end_word, OWS);
	}
	req.transfer_encoding.push_back(tmp_ext);
	while (extension[end_word] == ',' || isblank(extension[end_word]))
	  ++end_word;
	begin_word = end_word;
	tmp_ext.token.clear();
	tmp_ext.transfer_parameter.clear();
  }
}

void calculate_length_message(Request& req) {
  if (req.headers.count("transfer-encoding") && req.headers.count("content-length"))
	error(400);
  if (req.headers.count("transfer-encoding")) {
    if (req.transfer_encoding.back().token != "chunked") {
	  error(400);
    }
  } else if (req.headers.count("content-length") == 0) {
    req.content_length = 0;
  }
}

bool read_body(Request& req, std::string const &data, size_t begin) {
  bool answer = true;
  if (req.content_length == -1) {
	answer = decoding_chunked(req, data, begin);
  }
  else {
    uint64_t bytes_to_add = req.content_length;
    bytes_to_add -= req.body.length();
    if (bytes_to_add > data.length() - begin) {
      bytes_to_add = data.length() - begin;
      answer = false;
    }
	req.body.append(data, begin, bytes_to_add);
  }
  return answer;
}

/*
 * chunked-body = *chunk
 * last-chunk
 * trailer-part
 * CRLF
 *
 * chunk = chunk-size [ chunk-ext ] CRLF
 * chunk-data CRLF
 * last-chunk = 1*("0") [ chunk-ext ] CRLF
 *
 * chunk-data = 1*OCTET ; a sequence of chunk-size octets
 *
 * trailer-part = *( header-field CRLF )
 */
bool decoding_chunked(Request& req, std::string const &data, size_t begin) {
  size_t chunk_size;
  size_t pos = begin;
  // read chunk-size, chunk-ext(if any), and CRLF
  pos += read_chunk_size(chunk_size, data, pos);

  //read chunk-data
  while (chunk_size > 0) {
    //read chunk-data and CRLF
    if (data.length() - pos < chunk_size)
	  error(400);
    req.body.append(data, pos, chunk_size);
    pos += chunk_size;
    if (data.compare(pos, 2, "\r\n") != 0)
	  error(400);
    pos += 2;
    if (data.length() - pos == 0)
	  return false;
    pos += read_chunk_size(chunk_size, data, pos);
  }

  //read trailer field
  std::map<std::string, std::string> trailer_headers;
  parse_headers(trailer_headers, data, pos);
  return true;
}

/*
 * chunk-size = 1*HEXDIG
 *
 * chunk-ext = *( ";" chunk-ext-name [ "=" chunk-ext-val ] )
 * chunk-ext-name = token
 * chunk-ext-val = token / quoted-string
 */
size_t read_chunk_size(size_t& chunk_size, std::string const &data, size_t begin) {
  size_t pos = begin;
  std::string ext_name;
  std::string ext_val;

  //read chunk-size
  while (isxdigit(data[pos]))
	++pos;
  if (pos - begin == 0)
	error(400);
  std::stringstream ss(std::string(data, begin, pos - begin));
  ss >> std::hex >> chunk_size;
  if (ss.fail())
	error(400);

  // chunk-ext(if any)
  while (data[pos] == ';') {
	++pos;
	pos += get_token(ext_name, data, pos);
	if (data[pos] == '=') {
	  ++pos;
	  if (data[pos] == DQUOTE) {
		++pos;
		pos += get_quoted_string(ext_val, data, pos);
	  } else {
		pos += get_token(ext_name, data, pos);
	  }
	}
	ext_name.clear();
	ext_val.clear();
  }
  pos += skip_crlf(data, pos);
  return pos - begin;
}

/*
 * Host = uri-host [":" port]
 */
void host(Request& req, std::string const &value) {
  size_t host = 0;
  host += get_host(req.url.host, value, host);
  size_t port = host + 1;
  if (host != value.length()) {
	if (value[host] == ':') {
	  req.url.host += ":";
	  port += get_port(req.url.host, value, port);
	  if (port != value.length())
		error(400);
	} else {
	  error(400);
	}
  }
}

}
