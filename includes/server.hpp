#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <vector>
# include <map>
# include <location.hpp>

using namespace std;

typedef struct	s_err_page
{
	int		html_error_code;
	string	location;
}	err_page;

class Server
{

	private:
		vector<int>					_port;
		string						_server;
		vector<string>				_server_name;
		vector<err_page>			_error_pages;
		unsigned int				_max_body_size;
		vector<Location>			_locations;
		
		Server();

	public:
		~Server();
		Server(vector<string> input);
		Server(const Server &tbc);
		Server & operator=(const Server &tba);

		void 	call(const string& s, vector<string> val);
		int		parse_args(vector<string> arr, int i);
		void 	load_ports(vector<string> val);
		void 	load_ser_name(vector<string> val);
		void 	load_client_max_body_size(vector<string> val);
};

#endif
