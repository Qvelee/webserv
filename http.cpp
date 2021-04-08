#include "http.hpp"
#include <cstring>
#include "utility_http.hpp"
#include "error_http.hpp"
#include <sstream>

namespace http {

//const char* const CR = "\r"; // %x0D
const char *const CRLF = "\r\n";
//				  CTL = %x00-1F / %x7F ; isctrl()
//				  DIGIT = %x30-39 ; 0-9, isdigit()
const char        DQUOTE = '\"';
//				  HEXDIG = DIGIT / "A" / "B" / "C" / "D" / "E" / "F" / G" ;
//				  													isxdigit()
const char		  HTAB = '\t'; // %x09
//const char* const LF = "\n"; // %x0A
//				  LWSP = *(WSP / CRLF WSP);
//				  OCTET = %x00-FF ; isprint() || iscntrl()
const char		  SP = ' '; // %x20
//				  VCHAR = %x21-7E ; isgraph()
//				  WSP = SP / HTAB ; isblank()

//				  OWS = *(SP / HTAB)
//				  BWS = OWS
//				  RWS = 1*(SP/ HTAB)

// token = 1*tchar
// tchar = "!" / "#" / "$" / "%" / "&" / "’" / "*"
// / "+" / "-" / "." / "^" / "_" / "‘" / "|" / "~"
// / DIGIT / ALPHA

// quoted-string = DQUOTE *( qdtext / quoted-pair ) DQUOTE
// qdtext = HTAB / SP /%x21 / %x23-5B / %x5D-7E / obs-text
// obs-text = %x80-FF

// comment = "(" *( ctext / quoted-pair / comment ) ")"
// ctext = HTAB / SP / %x21-27 / %x2A-5B / %x5D-7E / obs-text

//quoted-pair = "\" ( HTAB / SP / VCHAR / obs-text )

// cписки генерировать так
// 1#elem => elem *(OWS "," OWS elem)
// #elem => [ 1#elem ]
// <n>#<m>element => element <n-1>*<m-1>( OWS "," OWS element )
// принимать так
// #element => [ ( "," / element ) *( OWS "," [ OWS element ] ) ]
// 1#element => *( "," OWS ) element *( OWS "," [ OWS element ] )



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
//  parse_message_body(bytes);
}

/*
 * request-line = method SP request-target SP HTTP-version CRLF
 */
void message::parse_request_line(char *&bytes) {
  char *begin_word = bytes;

  // method = token
  while (istchar(*bytes))
	++bytes;
  if (bytes == begin_word)
	error(400);
  if (bytes - begin_word > 15)
	error(501);
  if (*bytes != SP)
	error(400);
  start_line_.method_.assign(begin_word, bytes - begin_word);

  // SP
  ++bytes;
  begin_word = bytes;

  /*
   *
   */
  while (*bytes != SP && *bytes != '\0')
	++bytes;
  if (*bytes != SP)
	error(400);
  if (bytes - begin_word > 8000)
	error(414);
  start_line_.request_target_.assign(begin_word, bytes - begin_word);

  // SP
  ++bytes;
  begin_word = bytes;

  /*
   * HTTP-version = HTTP-name "/" DIGIT "." DIGIT
   * HTTP-name = %x48.54.54.50 ; "HTTP", case-sensitive
   */

  if (strncmp(bytes, "HTTP/", 5) != 0)
	error(400);
  bytes += 5;
  if (!isdigit(*bytes))
	error(400);
  ++bytes;
  if (*bytes != '.')
	error(400);
  ++bytes;
  if (!isdigit(*bytes))
	error(400);
  ++bytes;
  start_line_.http_version.assign(begin_word, bytes - begin_word);

  // CRLF
  if (strncmp(bytes, "\r\n", 2) != 0)
	error(400);
  bytes += 2;
}

/*
 * header-field = field-name ":" OWS field-value OWS
 *
 * field-name = token
 * field-value = *( field-content / obs-fold )
 * field-content = field-vchar [ 1*( SP / HTAB ) field-vchar ]
 * field-vchar = VCHAR / obs-text
 *
 * obs-fold = CRLF 1*( SP / HTAB )
 */
void message::parse_headers(char *&bytes) {
  char *begin_word = bytes;
  std::string field_name;
  std::string field_value;

  if (*bytes == SP)
	error(400);

  while (strncmp(CRLF, bytes, 2) != 0) {
	// token
	while (istchar(*bytes))
	  ++bytes;
	if (bytes == begin_word)
	  error(400);
	if (*bytes != ':')
	  error(400);
	field_name.assign(begin_word, bytes - begin_word);
	tolower(field_name);
	// ":"
	++bytes;

	// OWS
	while (isblank(*bytes))
	  ++bytes;
	begin_word = bytes;

	// fields-value
	do {
	  bytes = begin_word;
	  while (*bytes < 0 || isgraph(*bytes) || isblank(*bytes))
		++bytes;
	  if (strncmp(CRLF, bytes, 2) != 0)
		error(400);
	  field_value.append(begin_word, bytes - begin_word);
	  field_value.append(1, SP);
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
}

void message::check_valid_name(const std::string& name) const {
  if (transfer_coding_registration.count(name) == 0)
	error(400);
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
	// token
	while (istchar(extension[end_word]) && end_word < extension.size())
	  ++end_word;
	if (begin_word == end_word)
	  error(400);
	tmp.token_.assign(extension, begin_word, end_word - begin_word);
	tolower(tmp.token_);
	check_valid_name(tmp.token_);
	begin_word = end_word;

	// OWS
	while (isblank(extension[end_word]) && end_word < extension.size())
	  ++end_word;

	// ";"
	if (end_word != extension.size() && extension[end_word] == ';') {
	  ++end_word;
	  // OWS
	  while (isblank(extension[end_word]) && end_word < extension.size())
		++end_word;
	  /*
	   * transfer-parameter = token BWS "=" BWS ( token / quoted-string )
	   *
	   * quoted-string = DQUOTE *( qdtext / quoted-pair ) DQUOTE
	   * qdtext = HTAB / SP /%x21 / %x23-5B / %x5D-7E / obs-text
	   * obs-text = %x80-FF
	   *
	   * quoted-pair = "\" ( HTAB / SP / VCHAR / obs-text )
	   */
	  begin_word = end_word;
	  // token
	  while (istchar(extension[end_word]) && end_word < extension.size())
		++end_word;
	  if (begin_word == end_word)
		error(400);
	  tmp.transfer_parameter_.name_.assign(extension, begin_word, end_word -
	  begin_word);
	  // BWS
	  while (isblank(extension[end_word]) && end_word < extension.size())
		++end_word;
	  // "="
	  if (end_word >= extension.size() || extension[end_word] != '=')
		error(400);
	  ++end_word;
	  // BWS
	  while (isblank(extension[end_word]) && end_word < extension.size())
		++end_word;
	  begin_word = end_word;
	  if (begin_word >= extension.size())
		error(400);
	  // quoted-string / token
	  if (extension[end_word] == DQUOTE) {
	    ++end_word;
		++begin_word;
		while (end_word < extension.size() && (isqdtext(extension[end_word])
		|| extension[end_word] == '\\')) {
		  if (extension[end_word] == '\\') {
		    tmp.transfer_parameter_.value_.append(extension, begin_word,
											end_word - begin_word);
		    ++end_word;
		    if (end_word == extension.size())
			  error(400);
		    if (extension[end_word] != HTAB && extension[end_word] != SP &&
		    !isgraph(extension[end_word]) && extension[end_word] >= 0)
			  error(400);
		    tmp.transfer_parameter_.value_.append(1, extension[end_word]);
		    ++end_word;
		    begin_word = end_word;
		  } else {
			++end_word;
		  }
		}
		if (end_word == extension.size() || extension[end_word] != '"')
		  error(400);
		else
		  tmp.transfer_parameter_.value_.append(extension, begin_word,
										  end_word - begin_word);
		++end_word;
		begin_word = end_word;
	  } else {
	    while (istchar(extension[end_word]) && end_word < extension.size())
		  ++end_word;
	    tmp.transfer_parameter_.value_.assign(extension, begin_word, end_word
	    - begin_word);
	  }
	}
	message_info_.transfer_coding_.push_back(tmp);
	while (end_word != extension.size() && (extension[end_word] == ',' ||
	isblank(extension[end_word])))
	  ++end_word;
	begin_word = end_word;
	tmp.token_.clear();
	tmp.transfer_parameter_.name_.clear();
	tmp.transfer_parameter_.value_.clear();
  }
}
}