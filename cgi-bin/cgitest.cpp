# include <unistd.h>
# include <string>
# include <iostream>

using	std::cout;
using	std::cin;

int		main(int argc, char **argv, char **envp)
{
	std::string	body = "This is body.\nSome text.....\n";
	std::cerr << "cpptest started\n";
	cout << "Content-length: "<< body.length() << "\n";
	cout << "Content-type: text/plain\n";
	cout << "Status: 200 OK\n\n";
	cout << body;
	std::string	buffer;
	while (std::getline(cin, buffer))
		std::cout << buffer << std::endl;
	std::cerr << "cpptest finished\n";
	return EXIT_SUCCESS;
}
