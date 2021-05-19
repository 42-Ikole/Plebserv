#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <vector>
# include <string>

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

typedef struct	s_lim_except
{
	bool					allow;
	vector<http_methods>	methods;
}				lim_except;

class Location
{
	private:
		vector<http_methods>	_methods;
		vector<lim_except>		_limit_except;
		string					_root;
		bool					_auto_index;
		vector<string>			_index_page;
		Location();
	
	public:
		~Location();
		Location(vector<string> input);
		Location(const Location &tbc);
		Location & operator=(const Location &tba);
};

#endif
