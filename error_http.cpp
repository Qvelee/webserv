#include "error_http.hpp"

namespace http {
	void error(int code)
	{
		throw code;
	}
}
