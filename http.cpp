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
bool	parse_request(Request& req, const char *bytes) {
  bytes += parse_request_line(req, bytes);
  bytes += parse_headers(req.headers, bytes);
  header_analysis(req, req.headers);
  calculate_length_message(req);
//  read_message_body(bytes);
  return true;
}

void	parse_request_target(URL &, const std::string &) {
//  if (src.length() > 8000)
//    error(414);
}

void	parse_and_validate_method(Method &, const std::string &) {

}

/*
 * request-line = method SP request-target SP HTTP-version CRLF
 */
std::size_t parse_request_line(Request &r, const char *bytes) {
  std::size_t size = 0;
  //skip first CRLF (3.5)
  if (strncmp("\r\n", bytes + size, 2) == 0)
    size += skip_crlf(bytes);
  std::string method;
  size += get_token(method, bytes + size);
  parse_and_validate_method(r.method, method);
  size += skip_space(bytes + size, SP);
  std::string request_target;
  size += get_request_target(r.url, bytes + size);
  parse_request_target(r.url, request_target);
  size += skip_space(bytes + size, SP);
  size += get_http_version(r.proto, bytes + size);
  if (r.proto != "HTTP/1.1")
	error(505);
  size += skip_crlf(bytes + size);
  return size;
}

/*
 * header-field = field-name ":" OWS field-value OWS
 */
std::size_t parse_headers(Headers &dst, const char *bytes) {
  std::size_t size = 0;
  std::string field_name;
  std::string field_value;

  if (*(bytes + size) == SP)
	error(400);

  while (strncmp("\r\n", bytes + size, 2) != 0) {
    // field-name = token
	size += get_token(field_name, bytes + size);
	tolower(field_name);

	if (*(bytes + size) != ':')
	  error(400);
	++size;

	size += skip_space(bytes + size, OWS);

	// field-value = *( field-content / obs-fold )
	// field-content = field-vchar [ 1*( SP / HTAB ) field-vchar ]
	// field-vchar = VCHAR / obs-text

	// obs-fold = CRLF 1*( SP / HTAB )
	bool obs_fold = false;
	do {
	  std::size_t local_size = 0;
	  while (*(bytes + size + local_size) < 0 || isgraph(*(bytes + size + local_size)
	  ) || isblank(*(bytes + size + local_size)))
		++local_size;
	  if (strncmp("\r\n", bytes + size + local_size, 2) != 0)
		error(400);
	  field_value.append(bytes + size, local_size);
	  field_value.append(1, ' ');
	  size += local_size;
	  size += 2;
	  if (isblank(*(bytes + size)))
	    obs_fold = true;
	  else
	    obs_fold = false;
	  while (isblank(*(bytes + size)))
	    ++size;
	} while (obs_fold);

	// OWS
	while (isblank(*field_value.rbegin()))
	  field_value.erase(field_value.end() - 1);

	if (dst.count(field_name) == 1)
	  dst[field_name].append(",");
	dst[field_name].append(field_value);
	field_value.clear();
	field_name.clear();
  }
  size += 2;
  return size;
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
  std::size_t begin_word = 0;
  std::size_t end_word = 0;
  transfer_extension tmp_ext;
  transfer_parameter tmp_par;

  while (begin_word != extension.size()) {
	end_word += get_token(tmp_ext.token, extension.c_str() + begin_word);
	tolower(tmp_ext.token);
	validate_transfer_coding(tmp_ext.token);

	end_word += skip_space(extension.c_str() + end_word, OWS);

	while (extension[end_word] == ';') {
	  ++end_word;

	  end_word += skip_space(extension.c_str() + end_word, OWS);
	  /*
	   * transfer-parameter = token BWS "=" BWS ( token / quoted-string )
	   */
	  begin_word = end_word;
	  end_word += get_token(tmp_par.name, extension.c_str() +
	  	begin_word);

	  end_word += skip_space(extension.c_str() + end_word, BWS);

	  if (extension[end_word] != '=')
		error(400);
	  ++end_word;

	  end_word += skip_space(extension.c_str() + end_word, BWS);

	  begin_word = end_word;
	  //quoted-string / token
	  if (extension[end_word] == DQUOTE) {
	    ++end_word;
		++begin_word;
		end_word += get_quoted_string(tmp_par.value,
								extension.c_str() + begin_word);
	  } else {
	    end_word += get_token(tmp_par.value, extension.c_str
	    () + begin_word);
	  }
	  tmp_ext.transfer_parameter.push_back(tmp_par);
	  tmp_par.name.clear();
	  tmp_par.value.clear();
	  end_word += skip_space(extension.c_str() + end_word, OWS);
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

//void message::read_message_body(const char *bytes) {
//  if (message_info_.length_ == "chunked")
//    decoding_chunked(bytes);
//  else if (message_info_.length_ == "content-length") {
//    if (strlen(bytes) >= message_info_.content_length_) {
//	  source_data_.decoded_body_.assign(bytes, message_info_.content_length_);
//	} else {
//	  error(400);
//	}
//  }
//}
//
///*
// * chunked-body = *chunk
// * last-chunk
// * trailer-part
// * CRLF
// *
// * chunk = chunk-size [ chunk-ext ] CRLF
// * chunk-data CRLF
// * last-chunk = 1*("0") [ chunk-ext ] CRLF
// *
// * chunk-data = 1*OCTET ; a sequence of chunk-size octets
// *
// * trailer-part = *( header-field CRLF )
// */
//void message::decoding_chunked(const char *bytes) {
//  std::size_t chunk_size;
//
//  // read chunk-size, chunk-ext(if any), and CRLF
//  bytes += read_chunk_size(chunk_size, bytes);
//
//  //read chunk-data
//  while (chunk_size > 0) {
//    //read chunk-data and CRLF
//    if (strlen(bytes) < chunk_size)
//	  error(400);
//    source_data_.decoded_body_.append(bytes, chunk_size);
//    bytes += chunk_size;
//    if (strncmp("\r\n", bytes, 2) != 0)
//	  error(400);
//    bytes += 2;
//    bytes += read_chunk_size(chunk_size, bytes);
//  }
//
//  //read trailer field
//  std::map<std::string, std::string> trailer_headers;
//  parse_headers(trailer_headers, bytes);
//}
//
///*
// * chunk-size = 1*HEXDIG
// *
// * chunk-ext = *( ";" chunk-ext-name [ "=" chunk-ext-val ] )
// * chunk-ext-name = token
// * chunk-ext-val = token / quoted-string
// */
//std::size_t message::read_chunk_size(std::size_t& chunk_size, const char
//*bytes) {
//  std::size_t size = 0;
//  std::string ext_name;
//  std::string ext_val;
//
//  //read chunk-size
//  while (isxdigit(*(bytes + size)))
//	++size;
//  if (size == 0)
//	error(400);
//  std::stringstream ss(std::string(bytes, size));
//  ss >> std::hex >> chunk_size;
//  if (ss.fail())
//	error(400);
//
//  // chunk-ext(if any)
//  while (*(bytes + size) == ';') {
//	++size;
//	size += get_token(ext_name, bytes + size);
//	if (*(bytes + size) == '=') {
//	  ++size;
//	  if (*(bytes + size) == DQUOTE) {
//		++size;
//		size += get_quoted_string(ext_val, bytes + size);
//	  } else {
//		size += get_token(ext_name, bytes + size);
//	  }
//	}
//	ext_name.clear();
//	ext_val.clear();
//  }
//
//  size += skip_crlf(bytes + size);
//
//  return size;
//}

}
