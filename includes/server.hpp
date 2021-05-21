#ifndef SERVER_HPP
# define SERVER_HPP

#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <iostream>
# include <location.hpp>
# include <plebception.hpp>
# include <server.hpp>
# include <utilities.hpp>
# include <header.hpp>

using namespace std;

typedef struct	s_err_page
{
	int		html_error_code;
	string	location;
}	err_page;

class Server
{
	public:
		vector<int>					_port;
		string						_server;
		vector<string>				_server_identifier;
		vector<err_page>			_error_pages;
		size_t						_max_body_size;
		vector<Location>			_locations;
		
		Server();

	public:
		virtual ~Server();
		Server(vector<string> input);
		Server(const Server &tbc);
		Server & operator=(const Server &tba);

		void 	call(const string& s, vector<string> val);
		int		parse_args(vector<string> arr, int i);
		void 	load_ports(vector<string> val);
		void 	load_server_identifier(vector<string> val);
		void 	load_client_max_body_size(vector<string> val);
		void	load_locations(vector<string> val);
		void	load_error_page(vector<string> val);
		
		vector<string>	check_listen(string &val);
		void			check_port(string &val);
		void			check_servername(string &val);

		Location		*match_location(string path);
		string			create_response(Header h);
};

std::ostream &operator<<(std::ostream &out, Server const &value);


#endif
