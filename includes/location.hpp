#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <vector>
# include <iostream>
# include <map>
# include <string>
# include <iomanip>
# include <exception>
# include <header.hpp> 
# include <unistd.h>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <cgi.hpp>


class Server;
using namespace std;

enum http_methods
{
	GET,
	HEAD,
	POST,
	PUT,
	DELETE,
	CONNECT,
	OPTIONS,
	TRACE
};

enum auto_index
{
	OFF,
	ON
};

typedef struct	s_lim_except
{
	bool					allow;
	vector<http_methods>	methods;
}				lim_except;

class Location
{
	public:
		vector<http_methods>	_methods;
		vector<string>			_limit_except;
		string					_root;
		string					_upload_store;
		bool					_auto_index;
		vector<string>			_index_page;
		vector<Cgi>				_cgi;
		size_t					_max_body_size;
		string					_location;
		Location();
	
	public:
		virtual ~Location();
		Location(vector<string> input);
		Location(const Location &tbc);
		Location & operator=(const Location &tba);

		void 	call(const string& s, vector<string> val);
		int		parse_args(string str);
		void 	set_root(vector<string> val);
		void 	set_auto_index(vector<string> val);
		void 	set_index_page(vector<string> val);
		void 	set_limit_except(vector<string> val);
		void 	set_cgi_pass(vector<string> val);
		void	set_upload_store(vector<string> val);
		void	load_client_max_body_size(vector<string> val);
		bool	run_cgi(Header &h, vector<unsigned char> &body, string file_path, Server &ser, size_t &size);




		char	*read_file(size_t len);
		string	find_file(Header h, int &response_code);
};

std::ostream &operator<<(std::ostream &out, Location const &value);

#endif
