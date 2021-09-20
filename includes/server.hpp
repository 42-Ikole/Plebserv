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

#ifndef SERVER_HPP
# define SERVER_HPP

# include <algorithm>
# include <vector>
# include <string>
# include <map>
# include <iostream>

# include <location.hpp>
# include <plebception.hpp>
# include <server.hpp>
# include <utilities.hpp>
# include <header.hpp>
#include <connections.hpp>


using namespace std;

typedef struct	s_err_page
{
	int			html_error_code;
	string		location;
}				err_page;

class Server
{
	public:
		vector<string>				server_identifier;
		map<int, string>			error_pages;
		vector<Location>			locations;
		string						server;
		vector<int>					port;
		
		Server();

	public:
		virtual ~Server();
		Server(vector<string> input);
		Server(const Server& tbc);
		Server& operator=(const Server& tba);

		void 	call(const string& s, vector<string> val);
		int		parse_args(vector<string> arr, int i);
		void 	load_ports(vector<string> val);
		void 	load_server_identifier(vector<string> val);
		void	load_locations(vector<string> val);
		void	load_error_page(vector<string> val);
		
		vector<string>	check_listen(string& val);
		void			check_port(string& val);
		void			check_servername(string& val);

		Location		*match_location(string path);
		string	create_response(connect_data &data);

		void			err_code_file(string& body, int response_code);
		inline size_t 	get_error_file_len(int response_code);

		string	return_get(connect_data &data, Location* l);
		string	return_post(connect_data &data, Location* l);
		string	return_delete(connect_data &data, Location* l);
		string	return_options(connect_data &data, Location* l);
		string	return_put(connect_data &data, Location* l);
		string	return_head(connect_data &data, Location* l);
		string	return_connect(connect_data &data, Location* l);
		string	return_trace(connect_data &data, Location* l);

};

std::ostream &operator<<(std::ostream& out, Server const& value);

#endif
