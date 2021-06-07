#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <iostream>

#include <plebception.hpp>
#include <server.hpp>
#include <utilities.hpp>

typedef void (Server::*LoadFunction)(vector<string> val);

static map<string, LoadFunction> create_map()
{
	map<string, LoadFunction> m;
	m["location"] 				= &Server::load_locations;
	m["server_name"] 			= &Server::load_server_identifier;
	m["listen"]					= &Server::load_ports;
	m["error_page"]				= &Server::load_error_page;
	return m;
}

static map<string, LoadFunction> g_server_load_map = create_map();

Server::Server(Server const &tbc)
{
	*this = tbc;
}

Server::~Server() {}

Server	&Server::operator=(Server const &tba)
{
	_port = tba._port;
	_server = tba._server;
	_server_identifier = tba._server_identifier;
	_error_pages = tba._error_pages;
	_locations = tba._locations;
	return (*this);
}

std::ostream &operator<<(std::ostream &out, Server const &value)
{
	out << "------------ SERVER " << value._server << "--------" << std::endl;
	out << std::setw(15) << "LISTEN | ";
	for (size_t i = 0; i < value._port.size(); i++)
		out << value._port[i] << " ";
	out << std::endl << std::setw(15) << "IDENTIFERS | ";
	for (size_t i = 0; i < value._server_identifier.size(); i++)
		out << "{" << value._server_identifier[i] << "} ";
	out << std::endl << std::setw(15) << "ERROR PAGES | ";
	for (std::map<int, std::string>::const_iterator i = value._error_pages.begin(); i != value._error_pages.end(); ++i)
		out << "{" << i->first << ' ' << i->second << "} ";
	out << std::endl << std::setw(15) << "LOCATIONS | " << endl;
	for (size_t i = 0; i < value._locations.size(); i++)
		out << value._locations[i] << " ";
	out << std::endl << "---------------- DONE ----------------" << std::endl;
	return (out);
}

Server::Server(vector<string> input) :
	_server(""), _server_identifier()
{
	_server_identifier.push_back("");
	_port.push_back(80);
	_port.push_back(8000);
	for (size_t x = 1; x < input.size();)
		x = parse_args(input, x);
}

void	Server::call(const string& s, vector<string> val)
{
	LoadFunction func = g_server_load_map[s];
	if (func == 0)
		throw Plebception(ERR_UNKNOWN_IDENT, s, val[0]);
	(this->*func)(val);
}

void	Server::check_servername(string &val)
{
	vector<string> tokens = ft::split(val, ".");
	if ((tokens.size() == 3 && tokens[0] != "www") || tokens.size() > 3)
		throw Plebception(ERR_INVALID_VALUE, "listen1", val);
	else if(tokens.size() == 3)
		tokens.erase(tokens.begin());
	string res = tokens[0];
	if (tokens.size() == 2)
		res += '.' + tokens[1];
	if (_server != "" && _server.find(res) == string::npos)
		throw Plebception(ERR_MULTIPLE_DOM, "listen2", res);
}

void	Server::check_port(string &val)
{
	if (val.find_first_not_of("0123456789") != string::npos)
		throw Plebception(ERR_INVALID_VALUE, "listen3", val);
}

vector<string>	Server::check_listen(string &val)
{
	vector<string> tmp = ft::split(val, ":");

	if (tmp.size() > 2)
		throw Plebception(ERR_INVALID_VALUE, "listen4", val);
	else if (tmp.size() == 2)
	{
		check_servername(tmp[0]);			
		check_port(tmp[1]);
	}
	else if (tmp[0].find_first_not_of("0123456789") != string::npos)
		check_servername(tmp[0]);
	else
		check_port(tmp[0]);
	return tmp;
}

void	Server::load_ports(vector<string> val)
{
	_port.clear();
	for (size_t i = 0; i < val.size(); i++)
	{
		vector<string> tmp = check_listen(val[i]);
		for (size_t j = 0; j < tmp.size(); j++)
		{
			if (tmp[j].find_first_not_of("0123456789") != string::npos)
				_server = tmp[j];
			else
				_port.push_back(ft::stoi(tmp[j]));
		}
	}
}

void	Server::load_server_identifier(vector<string> val)
{
	_server_identifier.clear();
	for (size_t i = 0; i < val.size(); i++)
	{
		check_servername(val[i]);
		_server_identifier.push_back(val[i]);
		if (_server == "")
			_server = val[i];
	}
}

void	Server::load_error_page(vector<string> val)
{
	if (val.size() > 2)
		throw Plebception(ERR_TOO_MANY_ARG, "error_page", val[2]);
	else if (val.size() < 2)
		throw Plebception(ERR_TOO_FEW_ARG, "error_page", val[0]);
	else if (val[0].find_first_not_of("0123456789") != string::npos)
		throw Plebception(ERR_INVALID_VALUE, "error_page", val[0]);
	int code = ft::stoi(val[0]);
	if (code < 100 || code >= 600)
		throw Plebception(ERR_OUT_OF_RANGE, "error_page", val[0]);

	struct stat st;
	if (stat(val[1].c_str(), &st) == -1 || !(st.st_mode & S_IFREG))
		throw Plebception(ERR_INVALID_ARG, "error_page", val[1]);
	_error_pages[code] = val[1];
}

void	Server::load_locations(vector<string> val)
{
	_locations.push_back(Location(val));
}

static bool verify_line(string s, char delim) {
	if (count(s.begin(), s.end(), delim) == 1 && s.find(delim) == s.size() - 1)
		return(true);
	return(false);
}

static void	validate_line(string token, string s)
{
	if (!verify_line(s, '{') && !verify_line(s, '}') && !verify_line(s, ';'))
		throw Plebception(ERR_INVALID_TOKEN, token, s);
}

static void trim_lines(vector<string> &args)
{
	for (size_t j = 0; j < args.size(); j++)
		if (args[j][args[j].size() - 1] == ';')
			args[j] = ft::trim_char(args[j], ';');
}

static void	parse_location_block(vector<string> &args, vector<string> &arr, vector<string> &tokens, size_t &i)
{
	args.push_back(tokens[1]);
	for(i++; i < arr.size() && arr[i].find('}') == string::npos; i++)
	{
		if (arr[i].length() == 0)
			continue ;
		validate_line(tokens[0], arr[i]);
		args.push_back(arr[i]);
	}
}

int Server::parse_args(vector<string> arr, int index)
{
	std::vector<string> tokens;
	std::vector<string> args;
	size_t i;

	for (i = index; i < arr.size(); i++)
	{
		string s = arr[i];
		if (!s.size())
			continue ;
		tokens = ft::split(s);
		if (tokens[0].length() == 1)
			return (i + 1);
		if (tokens.size() < 2)
			throw Plebception(ERR_TOO_FEW_ARG, tokens[0], "");
		validate_line(tokens[0], s);
		if (tokens[tokens.size() - 1] == "{")
			parse_location_block(args, arr, tokens, i);
		else
			for (size_t j = 1; j < tokens.size(); j++)
				args.push_back(tokens[j]);
		trim_lines(args);
		call(tokens[0], args);
		args.clear();
	}
	return(i);
}
