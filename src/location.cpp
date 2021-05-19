
#include <location.hpp>
#include <vector>
#include <error.hpp>
#include <map>

using namespace std;

typedef void (Location::*LoadFunction)(vector<string> val);

static map<string, LoadFunction> create_map()
{
	map<string, LoadFunction> m;
	m["root"]			= &Location::set_root;
	m["autoindex"]		= &Location::set_auto_index;
	m["index"]			= &Location::set_index_page;
	m["limit_except"]	= &Location::set_limit_except;
	return m;
}

map<string, LoadFunction> g_location_load_map = create_map();

Location::~Location()
{
	_methods.clear();
	_limit_except.clear();
	_index_page.clear();
}

Location::Location(const Location& tbc)
{
	*this = tbc;
}

void Location::set_root(vector<string> val)
{
	_root = val[0];
}

void Location::set_auto_index(vector<string> val)
{
	if (val[0] == "on;")
		_auto_index = on;
	else if (val[0] == "off;")
		_auto_index = off;
}

void Location::set_index_page(vector<string> val)
{
	for (size_t i = 0; i < val.size(); i++)
	{
		_index_page.push_back(val[i]);
	}
}

void Location::set_limit_except(vector<string> val)
{
	for (size_t i = 0; i < val.size(); i++)
	{
		_limit_except.push_back(val[i]);		
	}
}

Location& Location::operator=(Location const& tba)
{
	_methods = tba._methods;
	_limit_except = tba._limit_except;
	_root = tba._root;
	_auto_index = tba._auto_index;
	_index_page = tba._index_page;
	return *this;
}

std::ostream &operator<<(std::ostream &out, Location const &value)
{
	out << "------------ LOCATION " << value._location << "--------" << std::endl;
	out << std::setw(20) << "ROOT | " << value._root << std::endl;
	out << std::setw(20) << "AUTO INDEX | " << (value._auto_index == false ? COLOR_RED : COLOR_GREEN) << value._auto_index << COLOR_RESET << std::endl;
	out << std::setw(20) << "INDEX PAGE | ";
	for (size_t i = 0; i < value._index_page.size(); i++)
	{
		out << value._index_page[i] << " ";
	}
	out << std::endl;
	out << std::setw(20) << "LIMIT EXCEPT | ";
	for (size_t i = 0; i < value._limit_except.size(); i++)
	{
		out << value._limit_except[i] << " ";
	}
	out << std::endl;
	out << "------------ DONE ------------" << std::endl;

	return (out);
}

int	Location::parse_args(vector<string> arr, int i)
{
	if (i >= arr.size())
		return (0);
	string s = arr[i];
	size_t pos = s.find(" ");
	string param = s.substr(0, pos);
	vector<string> args;

	s = s.substr(pos + 1, s.size());
	for (pos = s.find(" "); pos != string::npos; pos = s.find(" "))
	{
		args.push_back(s.substr(0, pos));
		s = s.substr(pos + 1, s.size());
	}
	args.push_back(s);
	std::cout << "Name:" << param << "$" << std::endl;
	for (std::vector<std::string>::iterator it = args.begin(); it != args.end(); it++)
	{
		std::cout << *it << "$" << std::endl;		
	}
	cout << "---------" << std::endl;
	call(param, args);
	return (1);
}

Location::Location(vector<string> val) //: _root("/www/"), _server("default name"), _max_body_size(16000)
{
	cout << "AHJAHAHAHHA WE ARE STARTING TO LOCATION" << std::endl;
	_location = val[0];
	for (size_t x = 1; x < val.size();)
	{
		std::cout << val[x] << "----" << std::endl;
		x += parse_args(val, x);
	}
	
}

void Location::call(const string& s, vector<string> val)
{
	LoadFunction func = g_location_load_map[s];
	if (func == 0)
		return;
	(this->*func)(val);
}
