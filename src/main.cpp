#include <plebception.hpp>
#include <vector>
#include <string>
#include <iostream>

#include <server.hpp>

std::vector<std::string> get_lines(std::string file);
std::vector<Server> load_config(std::string filename);
void	host_servers(vector<Server> serv);

int main(int argc, char **argv, char **env)
{
	std::string filename;
	if (argc == 1)
		filename = "configs/basic-server.conf";
	else
		filename = argv[1];
	std::vector<Server> l = load_config(filename);
	for (size_t i = 0; i < l.size(); i++)
		std::cout << l[i] << std::endl;
	host_servers(l);
	(void)env;
	return (0);
}
