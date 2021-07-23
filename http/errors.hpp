#ifndef WEBSERV_ERRORS_HPP
#define WEBSERV_ERRORS_HPP

#include "http.hpp"

namespace http {
void error200(const Request &req, Response &resp);
void error201(const Request &req, Response &resp);
void error301(const Request &req, Response &resp);
void error302(const Request &req, Response &resp);
void error303(const Request &req, Response &resp);
void error307(const Request &req, Response &resp);
void error400(const Request &req, Response &resp);
void error403(const Request &req, Response &resp);
void error404(const Request &req, Response &resp);
void error405(const Request &req, Response &resp);
void error408(const Request &req, Response &resp);
void error413(const Request &req, Response &resp);
void error414(const Request &req, Response &resp);
void error500(const Request &req, Response &resp);
void error501(const Request &req, Response &resp);
void error503(const Request &req, Response &resp);
void error505(const Request &req, Response &resp);
void add_length(Response &resp, bool chunked);
}
#endif
