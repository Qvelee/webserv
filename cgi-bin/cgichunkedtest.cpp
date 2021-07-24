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
	std::cerr << "cppchunkedtest started\n";

	string	buffer;
	std::cerr << "You send to me:\n";
	while (std::getline(cin, buffer))
		std::cout << buffer << std::endl;

	string	body_1 = "This is chunked body.\nI added here some text for no reason";
	string	body_2 = "\nSome text.....";

	string	body_3;
	AddVariables(envp, body_3);

	cout << "Content-type: text/plain\n";
	cout << "Status: 200 OK\n\n";
	cout << body_1;
	std::cerr << "Send first part:\n";
	sleep(30);

	cout << body_2;
	std::cerr << "Send second part:\n";

	std::cerr << "---END---\n";
	std::cerr << "cppchunkedtest finished\n";
	return EXIT_SUCCESS;
}
