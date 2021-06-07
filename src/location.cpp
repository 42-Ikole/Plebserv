#include <location.hpp>
#include <vector>
#include <map>
#include <utilities.hpp>
#include <plebception.hpp>

using namespace std;

typedef void (Location::*LoadFunction)(vector<string> val);

static const string methods[] = {"GET", "POST", "PUT", "HEAD", "DELETE", "OPTIONS", "TRACE", "PATCH"};

static map<string, LoadFunction> create_map()
{
	map<string, LoadFunction> m;
	m["root"]					= &Location::set_root;
	m["autoindex"]				= &Location::set_auto_index;
	m["index"]					= &Location::set_index_page;
	m["limit_except"]			= &Location::set_limit_except;
	m["cgi"]					= &Location::set_cgi_pass;
	m["upload_store"]			= &Location::set_upload_store;
	m["client_max_body_size"]	= &Location::load_client_max_body_size;
	return m;
}

static map<string, LoadFunction> g_location_load_map = create_map();

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
	struct stat st;

	if (val.size() > 1)
		throw Plebception(ERR_TOO_MANY_ARG, "root", val[1]);
	if (stat(val[0].c_str(), &st) == -1 || !(st.st_mode & S_IFDIR))
		throw Plebception(ERR_BAD_LOCATION, "set_root", val[0]);
	_root = val[0];
}

void Location::set_upload_store(vector<string> val)
{
	if (val.size() > 1)
		throw Plebception(ERR_TOO_MANY_ARG, "upload_store", val[1]);

	struct stat dir;
	if (stat(val[0].c_str(), &dir) == 0)
		_upload_store = val[0];
	else
		throw Plebception(ERR_NO_LOCATION, "upload_store", val[0]);
}

void Location::set_auto_index(vector<string> val)
{
	if (val.size() > 1)
		throw Plebception(ERR_TOO_MANY_ARG, "autoindex", val[1]);
	if (val[0] == "on")
		_auto_index = ON;
	else if (val[0] == "off")
		_auto_index = OFF;
	else
		throw Plebception(ERR_INVALID_VALUE, "autoindex", val[0]);
}

void Location::set_cgi_pass(vector<string> val)
{
	struct stat st;

	if (val.size() != 2)
		throw Plebception(ERR_INVALID_AMOUNT_ARG, "set_cgi_pass", val[0]);
	if (val[0][0] != '.')
		throw Plebception(ERR_INVALID_ARG, "set_cgi_pass", val[0]);
	if (stat(val[1].c_str(), &st) == -1 || !(st.st_mode & S_IFREG))
		throw Plebception(ERR_NO_LOCATION, "set_cgi_pass", val[1]);	
	_cgi.push_back(Cgi(val[1], val[0]));
}

void Location::set_index_page(vector<string> val)
{
	_index_page.clear();
	for (size_t i = 0; i < val.size(); i++)
		_index_page.push_back(val[i]);
}

void Location::set_limit_except(vector<string> val)
{
	for (size_t i = 0; i < val.size(); i++)
	{
		for (size_t x = 0; x < 8; x++)
		{
			if (val[i] == methods[x])
			{
				_limit_except.push_back(val[i]);		
				break;
			}
			if (x == 7)
				throw Plebception(ERR_INVALID_VALUE, "limit_execpt", val[i] + " not found!");
		}
	}
}

Location& Location::operator=(Location const& tba)
{
	_location		= tba._location;
	_methods		= tba._methods;
	_limit_except	= tba._limit_except;
	_root			= tba._root;
	_auto_index		= tba._auto_index;
	_index_page		= tba._index_page;
	_cgi			= tba._cgi;
	_upload_store	= tba._upload_store;
	_max_body_size	= tba._max_body_size;
	return *this;
}

std::ostream &operator<<(std::ostream &out, Location const &value)
{
	out << "------------ LOCATION " << value._location << " --------" << std::endl;
	out << std::setw(20) << "ROOT | " << value._root << std::endl;
	out << std::setw(20) << "MAX_BODY | " << value._max_body_size << std::endl;
	out << std::setw(20) << "UPLOAD STORE | " << value._upload_store << std::endl;
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
	out << std::setw(20) << "CGI PASS | ";
	for (size_t i = 0; i < value._cgi.size(); i++)
	{
		out << "{" << value._cgi[i] << "} ";
	}
	out << std::endl;
	out << "------------ DONE ------------" << std::endl;

	return (out);
}

int	Location::parse_args(string str)
{
	vector<string> args;
	vector<string> tokens;

	if (!str.size())
		return (0);
	tokens = ft::split(str);
	for (size_t i = 1; i < tokens.size(); i++)
		args.push_back(tokens[i]);
	call(tokens[0], args);
	return (1);
}

Location::Location(vector<string> val): _root("/html"), _upload_store("/html/uploads"), _auto_index(OFF), _max_body_size(16000), _location(val[0])
{
	_index_page.push_back("index.html");
	_index_page.push_back("index");

	if (val[0].find('{') != string::npos)
		throw Plebception(ERR_NO_VALUE, "location path", val[0]);
	for (size_t x = 1; x < val.size(); x++)
		if (!val[x].empty())
			parse_args(val[x]);
}

void Location::call(const string& s, vector<string> val)
{
	LoadFunction func = g_location_load_map[s];
	if (func == 0)
		throw Plebception(ERR_UNKNOWN_IDENT, s, val[0]);
	(this->*func)(val);
}

void	Location::load_client_max_body_size(vector<string> val)
{
	if (val.size() == 0)
		throw Plebception(ERR_NO_VALUE, "client_max_body_size", "");
	size_t	pos = val[0].find_first_not_of("0123456789");
	size_t	mul = 1;

	if (pos != string::npos)
	{
		if (val[0][pos] != *(val[0].end() - 1))
			throw Plebception(ERR_INVALID_VALUE, "client_max_body_size", val[0]);
		char c = val[0][pos];
		switch (c)
		{
			case 'k':
				mul = 1000; break;
			case 'm':
				mul = 1000000; break;
			case 'g':
				mul = 1000000000; break;
			default:
				throw Plebception(ERR_INVALID_VALUE, "client_max_body_size", val[0]);
		}
	}
	_max_body_size = ft::stoi(val[0]) * mul;
}

bool	Location::run_cgi(Header &h, string &body, string file_path, Server &ser, size_t &size)
{
	for (size_t i = 0; i < _cgi.size(); i++)
	{
		if (h._extension == _cgi[i]._match)
		{
			_cgi[i].cgi_response(h, body, file_path, ser, size);
			return (true);
		}
	}
	return (false);
}

bool	Location::method_allowed(Header &h, int &response_code)
{
	if (_limit_except.size())
	{
		size_t i = 0;
		for (; i < _limit_except.size() && h._method != _limit_except[i]; i++);
		if (i == _limit_except.size())
		{
			response_code = 405;
			throw Plebception("405 method not allowed", "find_file", h._path);
			return (false);
		}
	}
	return (true);
}
// /dir/hoi/jaja.txt

// /jaja.txt

string	Location::find_file(Header h, int &response_code)
{
	struct stat file_status;
	string full_path = _root + "/" + h._path.replace(h._path.find(_location), _location.size(), "");

	std::cout << full_path << std::endl;	
	if (stat(full_path.c_str(), &file_status) == -1)
	{
		response_code = 404;
		throw Plebception(ERR_NO_LOCATION, "find_file", full_path);
	}
	if (file_status.st_mode & S_IFREG)
		return (full_path);
	else if (file_status.st_mode & S_IFDIR)
	{
		if (full_path[full_path.size() - 1] != '/')
			full_path += '/';
		for (size_t i = 0; i < _index_page.size(); i++)
		{
			string new_path = full_path + _index_page[i];
			std::cout << "Testing path " << new_path << std::endl;
			if (stat(new_path.c_str(), &file_status) != -1)
				return (new_path);
		}
	}
	response_code = 404;
	throw (Plebception(ERR_NO_LOCATION, "find_file", full_path));
}
