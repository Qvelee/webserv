#include "errors.hpp"
#include <sstream>

namespace http {

const char* const BadRequest = "<html>\n"
"<head><title>400 Bad Request</title></head>\n"
"<body>\n"
"<center><h1>400 Bad Request</h1></center>\n"
"</body>\n"
"</html>";

const char* const Forbidden = "<html>\n"
							   "<head><title>403 Forbidden</title></head>\n"
							   "<body>\n"
							   "<center><h1>403 Forbidden</h1></center>\n"
							   "</body>\n"
							   "</html>";


const char* const NotFound = "<html>\n"
							  "<head><title>404 Not Found</title></head>\n"
							  "<body>\n"
							  "<center><h1>404 Not Found</h1></center>\n"
							  "</body>\n"
							  "</html>";

const char* const MethodNotAllowed = "<html>\n"
							  "<head><title>405 Method Not Allowed</title></head>\n"
							  "<body>\n"
							  "<center><h1>405 Method Not Allowed</h1></center>\n"
							  "</body>\n"
							  "</html>";

const char* const RequestTimeout = "<html>\n"
									 "<head><title>408 Request Timeout</title></head>\n"
									 "<body>\n"
									 "<center><h1>408 Request Timeout</h1></center>\n"
									 "</body>\n"
									 "</html>";

const char* const RequestEntityTooLarge = "<html>\n"
								   "<head><title>413 Request Entity Too "
								   "Large</title></head>\n"
								   "<body>\n"
								   "<center><h1>413 Request Entity Too "
								   "Large</h1></center>\n"
								   "</body>\n"
								   "</html>";


const char* const RequestURITooLong = "<html>\n"
										  "<head><title>414 Request URI Too Long"
										  "</title></head>\n"
										  "<body>\n"
										  "<center><h1>414 Request URI Too Long"
										  "</h1></center>\n"
										  "</body>\n"
										  "</html>";

const char* const InternalServerError = "<html>\n"
									  "<head><title>500 Internal Server Error"
									  "</title></head>\n"
									  "<body>\n"
									  "<center><h1>500 Internal Server Error"
									  "</h1></center>\n"
									  "</body>\n"
									  "</html>";

const char* const NotImplemented = "<html>\n"
									  "<head><title>501 Not Implemented"
									  "</title></head>\n"
									  "<body>\n"
									  "<center><h1>501 Not Implemented"
									  "</h1></center>\n"
									  "</body>\n"
									  "</html>";

const char* const ServiceUnavailable = "<html>\n"
									  "<head><title>503 Service Unavailable"
									  "</title></head>\n"
									  "<body>\n"
									  "<center><h1>503 Service Unavailable"
									  "</h1></center>\n"
									  "</body>\n"
									  "</html>";

const char* const HTTPVersionNotSupported = "<html>\n"
									  "<head><title>505 HTTP Version Not Supported"
									  "</title></head>\n"
									  "<body>\n"
									  "<center><h1>505 HTTP Version Not Supported"
									  "</h1></center>\n"
									  "</body>\n"
									  "</html>";

void error200(const Request &, Response &resp) {
  resp.status = "OK";
  std::stringstream ss;
  ss << resp.body.length();
  std::string length;
  ss >> length;
  resp.header["Content-length"] = length;
}

void error201(const Request &, Response &resp) {
  resp.status = "Created";
  std::stringstream ss;
  ss << resp.body.length();
  std::string length;
  ss >> length;
  resp.header["Content-length"] = length;
}

void error301(const Request &req, Response &resp) {
  resp.status = "Moved Permanently";
  resp.header["Location"] = req.serv_config.redirection_url;
  resp.header["Content-length"] = "0";
}

void error302(const Request &req, Response &resp) {
  resp.status = "Found";
  resp.header["Location"] = req.serv_config.redirection_url;
  resp.header["Content-length"] = "0";
}

void error303(const Request &req, Response &resp) {
  resp.status = "See Other";
  resp.header["Location"] = req.serv_config.redirection_url;
  resp.header["Content-length"] = "0";
}

void error307(const Request &req, Response &resp) {
  resp.status = "Temporary Redirect";
  resp.header["Location"] = req.serv_config.redirection_url;
  resp.header["Content-length"] = "0";
}

void error400(const Request &req, Response &resp) {
  resp.status = "Bad Request";
  if (req.serv_config.error_pages.count(400)) {
	resp.error_file = req.serv_config.error_pages.at(400);
  } else {
    resp.body = BadRequest;
	std::stringstream ss;
	ss << resp.body.length();
	std::string length;
	ss >> length;
    resp.header["Content-length"] = length;
  }
}

void error403(const Request &req, Response &resp) {
  resp.status = "Forbidden";
  if (req.serv_config.error_pages.count(403)) {
	resp.error_file = req.serv_config.error_pages.at(403);
  } else {
	resp.body = Forbidden;
	std::stringstream ss;
	ss << resp.body.length();
	std::string length;
	ss >> length;
	resp.header["Content-length"] = length;
  }
}

void error404(const Request &req, Response &resp) {
  resp.status = "Not Found";
  if (req.serv_config.error_pages.count(404)) {
	resp.error_file = req.serv_config.error_pages.at(404);
  } else {
	resp.body = NotFound;
	std::stringstream ss;
	ss << resp.body.length();
	std::string length;
	ss >> length;
	resp.header["Content-length"] = length;
  }
}

void error405(const Request &req, Response &resp) {
  resp.status = "Method Not Allowed";
  if (req.serv_config.error_pages.count(405)) {
	resp.error_file = req.serv_config.error_pages.at(405);
  } else {
	resp.body = MethodNotAllowed;
	std::stringstream ss;
	ss << resp.body.length();
	std::string length;
	ss >> length;
	resp.header["Content-length"] = length;
  }
}

void error408(const Request &req, Response &resp) {
  resp.status = "Request Timeout";
  if (req.serv_config.error_pages.count(408)) {
	resp.error_file = req.serv_config.error_pages.at(408);
  } else {
	resp.body = RequestTimeout;
	std::stringstream ss;
	ss << resp.body.length();
	std::string length;
	ss >> length;
	resp.header["Content-length"] = length;
  }
}

void error413(const Request &req, Response &resp) {
  resp.status = "Request Entity Too Large";
  if (req.serv_config.error_pages.count(413)) {
	resp.error_file = req.serv_config.error_pages.at(413);
  } else {
	resp.body = RequestEntityTooLarge;
	std::stringstream ss;
	ss << resp.body.length();
	std::string length;
	ss >> length;
	resp.header["Content-length"] = length;
  }
}

void error414(const Request &req, Response &resp) {
  resp.status = "Request URI Too Long";
  if (req.serv_config.error_pages.count(414)) {
	resp.error_file = req.serv_config.error_pages.at(414);
  } else {
	resp.body = RequestURITooLong;
	std::stringstream ss;
	ss << resp.body.length();
	std::string length;
	ss >> length;
	resp.header["Content-length"] = length;
  }
}

void error500(const Request &req, Response &resp) {
  resp.status = "Internal Server Error";
  if (req.serv_config.error_pages.count(500)) {
	resp.error_file = req.serv_config.error_pages.at(500);
  } else {
	resp.body = InternalServerError;
	std::stringstream ss;
	ss << resp.body.length();
	std::string length;
	ss >> length;
	resp.header["Content-length"] = length;
  }
}

void error501(const Request &req, Response &resp) {
  resp.status = "Not Implemented";
  if (req.serv_config.error_pages.count(501)) {
	resp.error_file = req.serv_config.error_pages.at(501);
  } else {
	resp.body = NotImplemented;
	std::stringstream ss;
	ss << resp.body.length();
	std::string length;
	ss >> length;
	resp.header["Content-length"] = length;
  }
}

void error503(const Request &req, Response &resp) {
  resp.status = "Service Unavailable";
  if (req.serv_config.error_pages.count(503)) {
	resp.error_file = req.serv_config.error_pages.at(503);
  } else {
	resp.body = ServiceUnavailable;
	std::stringstream ss;
	ss << resp.body.length();
	std::string length;
	ss >> length;
	resp.header["Content-length"] = length;
  }
}

void error505(const Request &req, Response &resp) {
  resp.status = "HTTP Version Not Supported";
  if (req.serv_config.error_pages.count(505)) {
	resp.error_file = req.serv_config.error_pages.at(505);
  } else {
	resp.body = HTTPVersionNotSupported;
	std::stringstream ss;
	ss << resp.body.length();
	std::string length;
	ss >> length;
	resp.header["Content-length"] = length;
  }
}

}
