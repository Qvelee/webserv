#ifndef CGIENVIROMENT_HPP
# define CGIENVIROMENT_HPP

#include <string>
#include <map>
#include <vector>

class CgiEnviroment{
	private:
		std::map<std::string, std::string> map_env;
	public:
		CgiEnviroment();
		virtual ~CgiEnviroment();
};

#endif
