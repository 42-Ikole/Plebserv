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

#ifndef CGI_HPP
#define CGI_HPP

#define FD_IN 0
#define FD_OUT 1

#include <string>
#include <header.hpp>
#include <struct_tmp.hpp>

class Server;

using namespace std;

class Cgi
{
public:
	string full_path;
	string match;
public:
	Cgi(string path, string match);
	Cgi(const Cgi& tbc);
	~Cgi();
	Cgi& operator=(const Cgi& tba);

	void	cgi_response(connect_data &data, string& body, string file_path, Server& ser, int &response_code);
	void	default_env(Header &h, string &body, string &file_path, Server &ser, map<string, string> &env_tmp);
	void	read_response(connect_data &data, char** env, string file_path);
	char*	create_env_var(string key, string value);
	void	cgi_child(cgi_session &sesh, char* args[3], char** env);
	void	cgi_parent(cgi_session &sesh);
	char	**create_env_array(map<string, string> &env);
};

int cgi_write(int &fdin, string &body, size_t &i);
int cgi_read(int &fdout, string &body);

std::ostream &operator<<(std::ostream& out, Cgi const& value);

#endif
