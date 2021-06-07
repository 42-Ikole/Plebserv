#include <plebception.hpp>
#include <vector>
#include <string>
#include <iostream>

#include <server.hpp>

std::vector<std::string> get_lines(std::string file);
std::vector<Server> load_config(std::string filename);
void	host_servers(vector<Server> serv);

static void	parser_test(char *filename)
{
	try
	{
		vector<Server> s = load_config(filename);
		// for (size_t i = 0; i < s.size(); i++)
		// 	std::cerr << s[i] << std::endl;
		std::cout << "[ " << COLOR_GREEN << "OK" << COLOR_RESET << " ]" << std::endl;
		exit(0);
	}
	catch(const std::exception& e)
	{
		std::cout << "[ " << COLOR_RED << "KO" << COLOR_RESET << " ]" << std::endl;
		exit(1);
	}
}

static void persistent_run_serv(int tries, std::vector<Server> &l)
{
	if (tries == 0)
	{
		cout << "Too many retries. shutting down..." << endl;
		exit(0);
	}
	try
	{
		host_servers(l);
	}
	catch(const Plebception &e)
	{
		std::cout << e.what() << std::endl;
		if (tries == -1)
			persistent_run_serv (-1, l);
		else
			persistent_run_serv (tries - 1, l);
	}
	catch (const Fatal &e)
	{
		std::cout << e.what() << std::endl;
	}
}

int main(int argc, char **argv)
{
	std::string filename;

	if (argc == 1)
		filename = "configs/basic-server.conf";
	else if (argc == 2)
		filename = argv[1];
	else if (argc == 3)
	{
		// load flags? maybe
		if (string(argv[1]) == "-t")
			parser_test(argv[2]);
	}
	std::vector<Server> l = load_config(filename);
	for (size_t i = 0; i < l.size(); i++)
		std::cout << l[i] << std::endl;
	persistent_run_serv(3, l);
	return (0);
}
