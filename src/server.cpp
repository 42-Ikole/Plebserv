#include <server.hpp>

#include <vector>
#include <string>
#include <iostream>

typedef void (Server::*LoadFunction)(vector<string> val);

inline map<string, LoadFunction> create_map()
{
	map<string, LoadFunction> m;
	m["location"] 				= &Server::load_locations;
	m["server_name"] 			= &Server::load_server_name;
	m["client_max_body_size"]	= &Server::load_client_max_body_size;
	m["listen"]					= &Server::load_ports;

	return m;
}

map<string, LoadFunction> g_load_map = create_map();

Server::Server(Server const &tbc)
{
	*this = tbc;
}

Server::~Server() {}

Server	&Server::operator=(Server const &tba)
{
	_port = tba._port;
	_server = tba._server;
	_server_name = tba._server_name;
	_error_pages = tba._error_pages;
	_max_body_size = tba._max_body_size;
	_locations = tba._locations;
	return (*this);
}

Server::Server(vector<string> input)
{
	for (size_t x = 1; x < input.size();)
	{
		std::cout << input[x] << "----" << std::endl;
		x += parse_args(input, x);
	}
	std::cout << "DIKKE ARIE" << std::endl;
}

void Server::call(const string& s, vector<string> val)
{
	LoadFunction func = g_load_map[s];
	(this->*func)(val);
}

void	Server::load_ports(vector<string> val)
{
	cerr << "PORTS" << endl;
}

void	Server::load_server_name(vector<string> val)
{
	cerr << "SERVER_NAME" << endl;
	_server_name.push_back(val[0]);
}

void	Server::load_client_max_body_size(vector<string> val)
{
	cerr << "MAX BODY SIZE" << endl;
	_server_name.push_back(val[0]);
}

void	Server::load_locations(vector<string> val)
{
	cerr << "LOCATIONS" << endl;
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
