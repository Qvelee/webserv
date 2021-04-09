#include "http.hpp"
#include <cstring>
#include "utility_http.hpp"
#include "error_http.hpp"
#include <sstream>

namespace http {

//				  CTL = %x00-1F / %x7F ; isctrl()
//				  DIGIT = %x30-39 ; 0-9, isdigit()
const char        DQUOTE = '\"';
//				  HEXDIG = DIGIT / "A" / "B" / "C" / "D" / "E" / "F" / G" ;
//				  													isxdigit()
//				  LWSP = *(WSP / CRLF WSP);
//				  OCTET = %x00-FF ; isprint() || iscntrl()
//				  VCHAR = %x21-7E ; isgraph()

// cписки генерировать так
// 1#elem => elem *(OWS "," OWS elem)
// #elem => [ 1#elem ]
// <n>#<m>element => element <n-1>*<m-1>( OWS "," OWS element )
// принимать так
// #element => [ ( "," / element ) *( OWS "," [ OWS element ] ) ]
// 1#element => *( "," OWS ) element *( OWS "," [ OWS element ] )







// comment = "(" *( ctext / quoted-pair / comment ) ")"
// ctext = HTAB / SP / %x21-27 / %x2A-5B / %x5D-7E / obs-text



const std::map<std::string, void (message::*)()>
	message::header_field_handlers = {
	{"content-length", &message::content_length},
	{"transfer-encoding", &message::transfer_encoding},
};

const std::map<std::string, int> message::transfer_coding_registration = {
	{"chunked", 1},
	{"compress", 1},
	{"deflate", 1},
	{"gzip", 1},
	{"x-compress", 1},
	{"x-gzip", 1},
};

/*
 * HTTP-message = start-line *( header-field CRLF ) CRLF [ message-body ]
 */
message::message(char *bytes) {
  parse_request_line(bytes);
  parse_headers(bytes);
  header_analysis();
  calculate_length_message();
  read_message_body(bytes);
}

/*
 * request-line = method SP request-target SP HTTP-version CRLF
 */
void message::parse_request_line(char *&bytes) {
  //skip first CRLF (3.5)
  if (strncmp("\r\n", bytes, 2) == 0)
    bytes += skip_crlf(bytes);
  bytes += get_token(start_line_.method_, bytes);
//  validate_method();
  bytes += skip_space(bytes, SP);
  bytes += get_request_target(start_line_.request_target_, bytes);
//  if (bytes - begin_word > 8000)
//	error(414);
  bytes += skip_space(bytes, SP);
  bytes += get_http_version(start_line_.http_version, bytes);
  bytes += skip_crlf(bytes);
}

/*
 * header-field = field-name ":" OWS field-value OWS
 */
void message::parse_headers(char *&bytes) {
  char *begin_word = bytes;
  std::string field_name;
  std::string field_value;

  if (*bytes == SP)
	error(400);

  while (strncmp("\r\n", bytes, 2) != 0) {
    // field-name = token
	bytes += get_token(field_name, bytes);
	tolower(field_name);

	if (*bytes != ':')
	  error(400);
	++bytes;

	bytes += skip_space(bytes, OWS);
	begin_word = bytes;

	// field-value = *( field-content / obs-fold )
	// field-content = field-vchar [ 1*( SP / HTAB ) field-vchar ]
	// field-vchar = VCHAR / obs-text

	// obs-fold = CRLF 1*( SP / HTAB )
	do {
	  bytes = begin_word;
	  while (*bytes < 0 || isgraph(*bytes) || isblank(*bytes))
		++bytes;
	  if (strncmp("\r\n", bytes, 2) != 0)
		error(400);
	  field_value.append(begin_word, bytes - begin_word);
	  field_value.append(1, ' ');
	  begin_word = bytes + 2;
	  while (isblank(*begin_word))
		++begin_word;
	} while (isblank(*(bytes + 2)));

	bytes = begin_word;
	// OWS
	while (isblank(*field_value.rbegin()))
	  field_value.erase(field_value.end() - 1);

	if (headers_.count(field_name) == 1)
	  headers_[field_name].append(",");
	headers_[field_name].append(field_value);
	field_value.clear();
	field_name.clear();
  }
  bytes += 2;
}

void message::header_analysis() {
  std::map<std::string, std::string>::const_iterator first;
  std::map<std::string, std::string>::const_iterator last;
  first = headers_.begin();
  last = headers_.end();

  for (; first != last; ++first) {
	if (header_field_handlers.count(first->first) == 1)
	  (this->*header_field_handlers.at(first->first))();
  }
}

/*
 * Content-Length = 1*DIGIT
 */
void message::content_length() {
  std::string &tmp = headers_["content-length"];

  if (tmp.find_first_not_of("0123456789") != std::string::npos)
	error(400);
  std::stringstream ss(tmp);
  ss >> message_info_.content_length_;
  if (ss.fail())
	error(400);
  if (message_info_.content_length_ < 0)
	error(400);
}

void message::validate_transfer_coding(const std::string& name) {
  if (transfer_coding_registration.count(name) == 0)
	error(501);
}

/*
 * transfer-extension = token *( OWS ";" OWS transfer-parameter )
 */
void message::transfer_encoding() {
  std::size_t begin_word = 0;
  std::size_t end_word = 0;
  std::string &extension = headers_["transfer-encoding"];
  transfer_extension tmp;

  while (begin_word != extension.size()) {
	end_word += get_token(tmp.token_, extension.c_str() + begin_word);
	tolower(tmp.token_);
	validate_transfer_coding(tmp.token_);

	end_word += skip_space(extension.c_str() + end_word, OWS);

	if (extension[end_word] == ';') {
	  ++end_word;
	  end_word += skip_space(extension.c_str() + end_word, OWS);
	  /*
	   * transfer-parameter = token BWS "=" BWS ( token / quoted-string )
	   */
	  begin_word = end_word;
	  end_word += get_token(tmp.transfer_parameter_.name_, extension.c_str() +
	  	begin_word);

	  end_word += skip_space(extension.c_str() + end_word, BWS);

	  if (extension[end_word] != '=')
		error(400);
	  ++end_word;

	  end_word += skip_space(extension.c_str() + end_word, BWS);

	  begin_word = end_word;
	  // quoted-string / token
	  if (extension[end_word] == '\"') {
	    ++end_word;
		++begin_word;
		end_word += get_quoted_string(tmp.transfer_parameter_.value_,
								extension.c_str() + begin_word);
	  } else {
	    end_word += get_token(tmp.transfer_parameter_.value_, extension.c_str
	    () + begin_word);
	  }
	}
	message_info_.transfer_coding_.push_back(tmp);
	while (extension[end_word] == ',' || isblank(extension[end_word]))
	  ++end_word;
	begin_word = end_word;
	tmp.token_.clear();
	tmp.transfer_parameter_.name_.clear();
	tmp.transfer_parameter_.value_.clear();
  }
}

void message::calculate_length_message() {
  if (headers_.count("transfer-encoding") == 1 &&
  headers_.count("content-length") == 1)
	error(400);
  if (headers_.count("transfer-encoding") == 1) {
    if (message_info_.transfer_coding_.back().token_ == "chunked") {
	  message_info_.length_ = "chunked";
	} else {
	  error(400);
    }
  } else if (headers_.count("content-length") == 1) {
    message_info_.length_ = "content-length";
  } else {
    message_info_.length_ = "empty";
  }
}

void message::read_message_body(char *&bytes) {
  if (message_info_.length_ == "chunked")
    decoding_chunked(bytes);
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
void message::decoding_chunked(char *&bytes) {
  std::size_t chunk_size;

  // read chunk-size, chunk-ext(if any), and CRLF
  chunk_size = read_chunk_size(bytes);

  //read chunk-data
  while (chunk_size > 0) {
    //read chunk-data and CRLF
    if (strlen(bytes) < chunk_size)
	  error(400);
    decoded_body_.append(bytes, chunk_size);
    bytes += chunk_size;
    if (strncmp("\r\n", bytes, 2) != 0)
	  error(400);
    chunk_size = read_chunk_size(bytes);
  }

  //read trailer field
  parse_headers(bytes);
}

/*
 * chunk-size = 1*HEXDIG
 *
 * chunk-ext = *( ";" chunk-ext-name [ "=" chunk-ext-val ] )
 * chunk-ext-name = token
 * chunk-ext-val = token / quoted-string
 */
std::size_t message::read_chunk_size(char *&bytes) {
  char *end_word = bytes;
  std::size_t chunk_size;
  std::string ext_name;
  std::string ext_val;

  //read chunk-size
  while (isxdigit(*end_word))
	++end_word;
  if (end_word == bytes)
	error(400);
  std::stringstream ss(std::string(bytes, end_word - bytes));
  ss >> std::hex >> chunk_size;
  if (ss.fail())
	error(400);
  bytes = end_word;

  // chunk-ext(if any)
  while (*bytes == ';') {
	++bytes;
	bytes += get_token(ext_name, bytes);
	if (*bytes == '=') {
	  ++bytes;
	  if (*bytes == DQUOTE) {
		++bytes;
		bytes += get_quoted_string(ext_val, bytes);
	  }
	} else {
	  bytes += get_token(ext_name, bytes);
	}
  }

  bytes += skip_crlf(bytes);

  return chunk_size;
}

}
