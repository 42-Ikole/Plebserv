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
	m["client_max_body_size"]	= &Server::load_client_max_body_size;
	m["listen"]					= &Server::load_ports;

	return m;
}

map<string, LoadFunction> g_server_load_map = create_map();

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
	_max_body_size = tba._max_body_size;
	_locations = tba._locations;
	return (*this);
}

std::ostream &operator<<(std::ostream &out, Server const &value)
{
	out << "------------ SERVER " << value._server << "--------" << std::endl;
	out << std::setw(15) << "LISTEN | ";
	for (size_t i = 0; i < value._port.size(); i++)
		out << value._port[i] << " ";
	out << std::endl;
	out << std::setw(15) << "BODY SIZE | " << value._max_body_size << std::endl;
	out << std::setw(15) << "IDENTIFERS | ";
	for (size_t i = 0; i < value._server_identifier.size(); i++)
		out << "{" << value._server_identifier[i] << "} ";
	out << std::endl;
	out << std::setw(15) << "LOCATIONS | " << endl;
	for (size_t i = 0; i < value._locations.size(); i++)
		out << value._locations[i] << " ";
	out << std::endl;

	out << "---------------- DONE ----------------" << std::endl;
	return (out);
}

Server::Server(vector<string> input) :
	_port({80, 8000}), _server("localhost"), _server_identifier(), _max_body_size(160000) 
{
	for (size_t x = 1; x < input.size();)
	{
		std::cout << input[x] << "----" << std::endl;
		x += parse_args(input, x);
	}
	std::cout << "DIKKE ARIE" << std::endl;
	std::cout << *this << std::endl;
}

void Server::call(const string& s, vector<string> val)
{
	LoadFunction func = g_server_load_map[s];
	if (func == 0)
		return;
	(this->*func)(val);
}

void	Server::load_ports(vector<string> val)
{
	cerr << "PORTS" << endl;
}

void	Server::load_server_identifier(vector<string> val)
{
	cerr << "SERVER_NAME" << endl;
	for (size_t i = 0; i < val.size(); i++)
		_server_identifier.push_back(trim_char(val[i], ';'));
}

void	Server::load_client_max_body_size(vector<string> val)
{
	cerr << "MAX BODY SIZE" << endl;
	val[0] = trim_char(val[0], ';');
	_max_body_size = std::atoi(val[0].c_str());
}

void	Server::load_locations(vector<string> val)
{
	cerr << "LOCATIONS" << endl;
	_locations.push_back(Location(val));
}


int	Server::parse_args(vector<string> arr, int i)
{
	if (i >= arr.size())
		return (0);
	string s = arr[i];
	size_t pos = s.find(" ");
	string param = s.substr(0, pos);
	vector<string> args;
	int x = 1;
	int depth = 1;

	s = s.substr(pos + 1, s.size());
	for (pos = s.find(" "); pos != string::npos; pos = s.find(" "))
	{
		args.push_back(s.substr(0, pos));
		s = s.substr(pos + 1, s.size());
		if (s.find('{') != string::npos)
		{
			while (depth)
			{
				s = arr[i + x++];
				if (s.find("{") != string::npos)
					depth++;
				else if (s.find("}") != string::npos)
					depth--;
				if (depth)
					args.push_back(s);
			}
			break;
		}
	}
	if (depth == 1)
		args.push_back(s);
	std::cout << "Name:" << param << "$" << std::endl;
	for (std::vector<std::string>::iterator it = args.begin(); it != args.end(); it++)
	{
		std::cout << *it << "$" << std::endl;		
	}
	cout << "---------" << std::endl;
	call(param, args);
	return (x);
}
