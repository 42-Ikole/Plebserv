#include "../includes/plebserv.hpp"

#include <iostream>

int main(int argc, char **argv, char **env)
{
	std::string filename;
	if (argc == 1)
	{
		// get default
	}
	else
		filename = argv[1];
	(void)argv;
	(void)env;
	return (0);
}
