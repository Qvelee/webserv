#include "http.hpp"
#include <cstring>
#include "utility_http.hpp"
#include <sstream>

namespace http {

const char	DQUOTE = '\"';

// HTTP-message = start-line *( header-field CRLF ) CRLF [ message-body ]
bool	parse_request(Request& req, std::string const &data) {
  size_t pos = 0;

  req.err = NoError;
  pos += parse_request_line(req, data, pos, req.err);
  if (req.err != NoError)
	return true;
  pos += parse_headers(req.headers, data, pos, req.err);
  if (req.err != NoError)
	return true;
  header_analysis(req, req.headers, req.err);
  if (req.err != NoError)
	return true;
  calculate_length_message(req, req.err);
  if (req.err != NoError)
	return true;
  if (req.content_length != 0) {
	bool answer = read_body(req, data, pos, req.err);
	if (req.err != NoError)
	  return true;
	return answer;
  }
  return true;
}

void	parse_request_target(url::URL &url, Method &, const std::string &str,
						  StatusCode &err) {
  if (str.length() > 8000) {
	err = StatusRequestURITooLong;
	return;
  }
//  if (method == OPTIONS) {
//    if (!parse_asterisk_form(url, str)) {
//      err = StatusBadRequest;
//    }
//  } else if (method == CONNECT) {
//    if (!parse_authority_form(url, str)) {
//      err = StatusBadRequest;
//    }
//  } else {
    if (!parse_origin_form(url, str)) {
      err = StatusBadRequest;
    }
//  }
}

void	parse_and_validate_method(Method &m, const std::string &str, StatusCode &err) {
  static std::map<const std::string, Method> methods = {
	  {"GET", GET},
//	  {"HEAD", HEAD},
	  {"POST", POST},
//	  {"PUT", PUT},
	  {"DELETE", DELETE},
//	  {"CONNECT", CONNECT},
//	  {"OPTIONS", OPTIONS},
//	  {"TRACE", TRACE},
  };
  if (methods.count(str)) {
    m = methods[str];
  } else {
    err = StatusNotImplemented;
  }
}


// request-line = method SP request-target SP HTTP-version CRLF
size_t parse_request_line(Request &r, std::string const &data, size_t begin, StatusCode &err) {
  size_t pos = begin;

  //skip first CRLF (3.5)
  if (data.compare(pos, 2, "\r\n") == 0)
  {
    pos += skip_crlf(data, pos, err);
    if (err != NoError)
	  return 0;
  }
  std::string method;
  pos += get_token(method, data, pos, err);
  if (err != NoError)
	return 0;
  parse_and_validate_method(r.method, method, err);
  if (err != NoError)
	return err;
  pos += skip_space(data, pos, SP, err);
  std::string request_target;
  pos += get_request_target(request_target, data, pos, err);
  if (err != NoError)
	return 0;
  parse_request_target(r.url, r.method, request_target, err);
  if (err != NoError)
	return 0;
  pos += skip_space(data, pos, SP, err);
  if (err != NoError)
	return 0;
  pos += get_http_version(r.proto, data, pos, err);
  if (err != NoError)
	return 0;
  if (r.proto != "HTTP/1.1") {
	return err = StatusHTTPVersionNotSupported;
  }
  pos += skip_crlf(data, pos, err);
  if (err != NoError)
	return 0;
  return pos - begin;
}

// header-field = field-name ":" OWS field-value OWS
size_t parse_headers(Headers &dst, std::string const &data, size_t begin, StatusCode &err) {
  size_t pos = begin;
  std::string field_name;
  std::string field_value;

  if (data[pos] == SP) {
	return err = StatusBadRequest;
  }
  while (data.compare(pos, 2, "\r\n") != 0) {
    // field-name = token
	pos += get_token(field_name, data, pos, err);
	if (err != NoError)
	  return 0;
	tolower(field_name);
	if (data[pos] != ':') {
	  return err = StatusBadRequest;
	}
	++pos;
	pos += skip_space(data, pos, OWS, err);
	if (err != NoError)
	  return 0;
	// field-value = *( field-content / obs-fold )
	// field-content = field-vchar [ 1*( SP / HTAB ) field-vchar ]
	// field-vchar = VCHAR / obs-text

	// obs-fold = CRLF 1*( SP / HTAB )
	bool obs_fold;
	do {
	  size_t local_size = 0;
	  while (data[pos + local_size] < 0 || isgraph(data[pos + local_size])
	   || isblank(data[pos + local_size]))
		++local_size;
	  if (data.compare(pos+local_size, 2, "\r\n") != 0) {
		return err = StatusBadRequest;
	  }
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
	  if (field_name == "host") {
		return err = StatusBadRequest;
	  }
	  dst[field_name].append(",");
	}
	dst[field_name].append(field_value);
	field_value.clear();
	field_name.clear();
  }
  pos += 2;
  return pos - begin;
}

void calculate_length_message(Request& req, StatusCode &err) {
  if (req.headers.count("transfer-encoding") && req.headers.count("content-length")) {
    err = StatusBadRequest;
	return;
  }
  if (req.headers.count("transfer-encoding")) {
    if (req.transfer_encoding.back().token != "chunked") {
	  err = StatusBadRequest;
      return;
    } else
    {
      req.content_length = -1;
    }
  } else if (req.headers.count("content-length") == 0) {
    req.content_length = 0;
  }
}

bool	add_body(Request &req, std::string const &data) {
  bool answer = read_body(req, data, 0, req.err);
  if (req.err != NoError)
	return true;
  return answer;
}

bool read_body(Request& req, std::string const &data, size_t begin, StatusCode &err) {
  bool answer = true;
  if (req.content_length == -1) {
	answer = decoding_chunked(req, data, begin, err);
	if (err != NoError)
	  return err;
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
bool decoding_chunked(Request& req, std::string const &data, size_t begin, StatusCode &err) {
  size_t chunk_size;
  size_t pos = begin;
  // read chunk-size, chunk-ext(if any), and CRLF
  pos += read_chunk_size(chunk_size, data, pos, err);
  if (err != NoError)
	return err;

  //read chunk-data
  while (chunk_size > 0) {
    //read chunk-data and CRLF
    if (data.length() - pos < chunk_size) {
	  return err = StatusBadRequest;
    }
    req.body.append(data, pos, chunk_size);
    pos += chunk_size;
    if (data.compare(pos, 2, "\r\n") != 0) {
	  return err = StatusBadRequest;
    }
    pos += 2;
    if (data.length() - pos == 0)
	  return false;
    pos += read_chunk_size(chunk_size, data, pos, err);
    if (err != NoError)
	  return err;
  }
  //read trailer field
  std::map<std::string, std::string> trailer_headers;
  parse_headers(trailer_headers, data, pos, err);
  return true;
}

/*
 * chunk-size = 1*HEXDIG
 *
 * chunk-ext = *( ";" chunk-ext-name [ "=" chunk-ext-val ] )
 * chunk-ext-name = token
 * chunk-ext-val = token / quoted-string
 */
size_t read_chunk_size(size_t& chunk_size, std::string const &data, size_t begin,
					   StatusCode &err) {
  size_t pos = begin;
  std::string ext_name;
  std::string ext_val;

  //read chunk-size
  while (isxdigit(data[pos]))
	++pos;
  if (pos - begin == 0) {
	return err = StatusBadRequest;
  }
  std::stringstream ss(std::string(data, begin, pos - begin));
  ss >> std::hex >> chunk_size;
  if (ss.fail()) {
	return err = StatusBadRequest;
  }
  // chunk-ext(if any)
  while (data[pos] == ';') {
	++pos;
	pos += get_token(ext_name, data, pos, err);
	if (err != NoError)
	  return err;
	if (data[pos] == '=') {
	  ++pos;
	  if (data[pos] == DQUOTE) {
		++pos;
		pos += get_quoted_string(ext_val, data, pos, err);
		if (err != NoError)
		  return err;
	  } else {
		pos += get_token(ext_name, data, pos, err);
		if (err != NoError)
		  return err;
	  }
	}
	ext_name.clear();
	ext_val.clear();
  }
  pos += skip_crlf(data, pos, err);
  if (err != NoError)
	return err;
  return pos - begin;
}

std::string get_response(const Request&, Response&) {
  std::string answer;
  answer.append("HTTP/1.1 200 OK\r\n"
				"Content-Length: 13\r\n"
				"Content-Type: text/plain\r\n"
				"\r\n"
				"Hello world!\n");
  return answer;
}

}
