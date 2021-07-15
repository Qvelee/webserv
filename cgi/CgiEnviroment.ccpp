#include "CgiEnviroment.hpp"

CgiEnviroment::CgiEnviroment()
{
	map_env.insert(make_pair("AUTH_TYPE", "Basic"));
	map_env.insert(make_pair("CONTENT_LENGTH", "-1"));//размер документа который содержит запрос
	map_env.insert(make_pair("CONTENT_TYPE", "text/html"));//need to change 
	map_env.insert(make_pair("GATEWAY_INTERFACE", "CGI/1.1"));
	map_env.insert(make_pair("PATH_INFO", ""));//все что после корневого каталога сервера, это то что нужно вернуть скрипт в случае успеха
	map_env.insert(make_pair("PATH_TRANSLATED", ""));//корневая директория плюс path_info
	map_env.insert(make_pair("QUERY_STRING", ""));//все что после ?
	map_env.insert(make_pair("REMOTE_ADDR", ""));//ip адресс удаленного узла
	map_env.insert(make_pair("REMOTE_IDENT", ""));//имя.www.(имя удаленного узла)
	map_env.insert(make_pair("REMOTE_USER", ""));//имя удаленного пользователя
	map_env.insert(make_pair("REQUEST_METHOD", ""));// get head post - тип http запроса
	map_env.insert(make_pair("REQUEST_URI", ""));//страница, отправившая запрос
	map_env.insert(make_pair("SCRIPT_NAME", ""));
	map_env.insert(make_pair("SERVER_NAME", ""));
	map_env.insert(make_pair("SERVER_PORT", ""));
	map_env.insert(make_pair("SERVER_PROTOCOL", ""));
	map_env.insert(make_pair("SERVER_SOFTWARE", ""));

}
