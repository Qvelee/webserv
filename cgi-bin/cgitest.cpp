# include <unistd.h>
# include <string>
# include <iostream>

using	std::cout;
using	std::cin;
using	std::string;

void	AddVariables(char **envp, string &body)
{
	int		i = -1;

	body.append("\nVariables:\n");
	while(envp[++i])
	{
		body.append(envp[i]);
		body.push_back('\n');
	}
}

int		main(int argc, char **argv, char **envp)
{
	std::cerr << "cpptest started\n";

	string	buffer;
	std::cerr << "You send to me:\n";
	while (std::getline(cin, buffer))
		std::cout << buffer << std::endl;
	int		i = -1;
	std::cerr << "Aruments: ";
	while (argv[++i])
		std::cerr << argv[i];
	std::cerr << " END-OF-ARGUMETNS\n";
	std::cerr << "---END---\n";

	string	body = "This is body.\nSome text.....\n";
	AddVariables(envp, body);
	cout << "Content-length: "<< body.length() << "\n";
	cout << "Content-type: text/plain\n";
	cout << "Status: 200 OK\n\n";
	cout << body;

	std::cerr << "cpptest finished\n";
	return EXIT_SUCCESS;
}
