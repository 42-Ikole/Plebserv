#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <vector>
# include <iostream>
# include <map>
# include <string>
# include <iomanip>
# include <exception>

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
	off,
	on
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
		bool					_auto_index;
		vector<string>			_index_page;
		string					_location;
		Location();
	
	public:
		~Location();
		Location(vector<string> input);
		Location(const Location &tbc);
		Location & operator=(const Location &tba);

		void 	call(const string& s, vector<string> val);
		int		parse_args(vector<string> arr, int i);
		void 	set_root(vector<string> val);
		void 	set_auto_index(vector<string> val);
		void 	set_index_page(vector<string> val);
		void 	set_limit_except(vector<string> val);
};

std::ostream &operator<<(std::ostream &out, Location const &value);

#endif
