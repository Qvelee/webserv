# include <unistd.h>
# include <string>
# include <iostream>

using	std::cout;
using	std::cin;

int		main(int argc, char **argv, char **envp)
{
	cout << "Hello from cpp cgi tester\n";
	cout << "You send to me " << std::endl;
	std::string	buffer;
	while (std::getline(cin, buffer))
		std::cout << buffer << std::endl;
	return EXIT_SUCCESS;
}
