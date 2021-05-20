#include <plebception.hpp>
#include <error.hpp>
#include <vector>
#include <string>
#include <iostream>

#include <server.hpp>

std::vector<std::string> get_lines(std::string file);
std::vector<Server> load_config(std::string filename);

int main(int argc, char **argv, char **env)
{
	std::string filename;
	if (argc == 1)
		filename = "configs/basic-server.conf";
	else
		filename = argv[1];
	
	std::vector<Server> l = load_config(filename);
	for (size_t i = 0; i < l.size(); i++)
	{
		std::cout << l[i] << std::endl;
	}
	
	// std::vector<std::string> res = get_lines(filename);
	// for (std::vector<std::string>::iterator i = res.begin(); i != res.end(); i++)
	// {
	// 	std::cout << *i << "$" << std::endl;
	// }

	(void)env;
	return (0);
}
