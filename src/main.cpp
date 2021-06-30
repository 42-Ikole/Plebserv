/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*	 																				 */
/*     _ (`-.              ('-. .-. .-')    .-')      ('-.  _  .-')        (`-.   	 */
/*    ( (OO  )           _(  OO)\  ( OO )  ( OO ).  _(  OO)( \( -O )     _(OO  )_ 	 */
/*   _.`     \ ,--.     (,------.;-----.\ (_)---\_)(,------.,------. ,--(_/   ,. \	 */
/*  (__...--'' |  |.-')  |  .---'| .-.  | /    _ |  |  .---'|   /`. '\   \   /(__/	 */
/*   |  /  | | |  | OO ) |  |    | '-' /_)\  :` `.  |  |    |  /  | | \   \ /   / 	 */
/*   |  |_.' | |  |`-' |(|  '--. | .-. `.  '..`''.)(|  '--. |  |_.' |  \   '   /, 	 */
/*   |  .___.'(|  '---.' |  .--' | |  \  |.-._)   \ |  .--' |  .  '.'   \     /__)	 */
/*   |  |      |      |  |  `---.| '--'  /\       / |  `---.|  |\  \     \   /    	 */
/*   `--'      `------'  `------'`------'  `-----'  `------'`--' '--'     `-'     	 */
/*																					 */
/* 									MADE BY											 */
/* 		—————————————————————————————————————————————————————————————————————		 */
/*				 Alpha_1337k       |    https://github.com/Alpha1337k				 */
/*				 VictorTennekes    |    https://github.com/VictorTennekes			 */
/*				 Kingmar	 	   |    https://github.com/K1ngmar					 */
/*																					 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <vector>
#include <string>
#include <iostream>

#include <plebception.hpp>
#include <server.hpp>

std::vector<std::string>	get_lines(std::string file);
std::vector<Server>			load_config(std::string filename);
void						host_servers(vector<Server> serv);

static void	parser_test(char* filename)
{
	try
	{
		vector<Server> s = load_config(filename);
		std::cerr << "[ " << COLOR_GREEN << "OK" << COLOR_RESET << " ]" << std::endl;
		exit(0);
	}
	catch(const std::exception& e)
	{
		std::cerr << "[ " << COLOR_RED << "KO" << COLOR_RESET << " ]" << std::endl;
		exit(1);
	}
}

static void persistent_run_serv(int tries, std::vector<Server>& l)
{
	if (tries == 0)
	{
		std::cerr << "Too many retries. shutting down..." << std::endl;
		exit(0);
	}
	try
	{
		host_servers(l);
	}
	catch(const Plebception& e)
	{
		std::cerr << e.what() << std::endl;
		if (tries == -1)
			persistent_run_serv (-1, l);
		else
			persistent_run_serv (tries - 1, l);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		exit (69);
	}
}

int main(int argc, char** argv)
{
	std::string			filename;
	std::vector<Server>	l;

	if (argc == 1)
		filename = "configs/basic-server.conf";
	else if (argc == 2)
		filename = argv[1];
	else if (argc == 3)
	{
		if (string(argv[1]) == "-t")
			parser_test(argv[2]);
	}
	try {
		l = load_config(filename);
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (-1);
	}
	for (size_t i = 0; i < l.size(); i++)
		std::cout << l[i] << std::endl;
	persistent_run_serv(3, l);
	return (0);
}
