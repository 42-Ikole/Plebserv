/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*	 																				 */
/*     _ (`-.              ('-. .-. .-')    .-')      ('-.  _  .-')        (`-.   	 */
/*    ( (OO  )           _(  OO)\  ( OO )  ( OO ).  _(  OO)( \( -O )     _(OO  )_ 	 */
/*   _.`     \ ,--.     (,------.;-----.\ (_)---\_)(,------.,------. ,--(_/   ,. \	 */
/*  (__...--'' |  |.-')  |  .---'| .-.  | /    _ |  |  .---'|   /`. '\   \   /(__/	 */
/*   |  /  | | |  | OO ) |  |    | '-' /_)\  :` `.  |  |    |  /  | | \   \ /   / 	 */
/*   |  |_.' | |  |`-' |(|  '--. | .-. `.  '..`''.)(|  '--. |  |_.' |  \   '   /, 	 */
/*   |  .___.'(|  '---.' |  .--' | |  \  |.-._)   \ |  .--' |  .  '.'   \     /__)	 */
/*   |  |      |      |  |  `---.| '--'  /\       / |  `---.|  |\  \     \   /    	 */
/*   `--'      `------'  `------'`------'  `-----'  `------'`--' '--'     `-'     	 */
/*																					 */
/* 									MADE BY											 */
/* 		—————————————————————————————————————————————————————————————————————		 */
/*				 Alpha_1337k       |    https://github.com/Alpha1337k				 */
/*				 VictorTennekes    |    https://github.com/VictorTennekes			 */
/*				 Kingmar	 	   |    https://github.com/K1ngmar					 */
/*																					 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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

Server::Server(Server const& tbc)
{
	*this = tbc;
}

Server::~Server() {}

Server	&Server::operator=(Server const& tba)
{
	port = tba.port;
	server = tba.server;
	server_identifier = tba.server_identifier;
	error_pages = tba.error_pages;
	locations = tba.locations;
	return (*this);
}

std::ostream &operator<<(std::ostream& out, Server const& value)
{
	out << "------------ SERVER " << value.server << "--------" << std::endl;
	out << std::setw(15) << "LISTEN | ";
	for (size_t i = 0; i < value.port.size(); i++)
		out << value.port[i] << " ";
	out << std::endl << std::setw(15) << "IDENTIFERS | ";
	for (size_t i = 0; i < value.server_identifier.size(); i++)
		out << "{" << value.server_identifier[i] << "} ";
	out << std::endl << std::setw(15) << "ERROR PAGES | ";
	for (std::map<int, std::string>::const_iterator i = value.error_pages.begin(); i != value.error_pages.end(); ++i)
		out << "{" << i->first << ' ' << i->second << "} ";
	out << std::endl << std::setw(15) << "LOCATIONS | " << endl;
	for (size_t i = 0; i < value.locations.size(); i++)
		out << value.locations[i] << " ";
	out << std::endl << "---------------- DONE ----------------" << std::endl;
	return (out);
}

Server::Server(vector<string> input) : server_identifier(), server("")
{
	server_identifier.push_back("");
	port.push_back(80);
	port.push_back(8000);
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

void	Server::check_servername(string& val)
{
	vector<string> tokens = ft::split(val, ".");
	if ((tokens.size() == 3 && tokens[0] != "www") || tokens.size() > 3)
		throw Plebception(ERR_INVALID_VALUE, "listen1", val);
	else if(tokens.size() == 3)
		tokens.erase(tokens.begin());
	string res = tokens[0];
	if (tokens.size() == 2)
		res += '.' + tokens[1];
	if (server != "" && server.find(res) == string::npos)
		throw Plebception(ERR_MULTIPLE_DOM, "listen2", res);
}

void	Server::check_port(string &val)
{
	if (val.find_first_not_of("0123456789") != string::npos)
		throw Plebception(ERR_INVALID_VALUE, "listen3", val);
}

vector<string>	Server::check_listen(string& val)
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
	port.clear();
	for (size_t i = 0; i < val.size(); i++)
	{
		vector<string> tmp = check_listen(val[i]);
		for (size_t j = 0; j < tmp.size(); j++)
		{
			if (tmp[j].find_first_not_of("0123456789") != string::npos)
				server = tmp[j];
			else
				port.push_back(ft::stoi(tmp[j]));
		}
	}
}

void	Server::load_server_identifier(vector<string> val)
{
	server_identifier.clear();
	for (size_t i = 0; i < val.size(); i++)
	{
		check_servername(val[i]);
		server_identifier.push_back(val[i]);
		if (server == "")
			server = val[i];
	}
}

void	Server::load_error_page(vector<string> val)
{
	struct stat st;

	if (val.size() > 2)
		throw Plebception(ERR_TOO_MANY_ARG, "error_page", val[2]);
	else if (val.size() < 2)
		throw Plebception(ERR_TOO_FEW_ARG, "error_page", val[0]);
	else if (val[0].find_first_not_of("0123456789") != string::npos)
		throw Plebception(ERR_INVALID_VALUE, "error_page", val[0]);

	int code = ft::stoi(val[0]);
	
	if (code < 100 || code >= 600)
		throw Plebception(ERR_OUT_OF_RANGE, "error_page", val[0]);
	if (stat(val[1].c_str(), &st) == -1 || !(st.st_mode & S_IFREG))
		throw Plebception(ERR_INVALID_ARG, "error_page", val[1]);
	error_pages[code] = val[1];
}

void	Server::load_locations(vector<string> val)
{
	locations.push_back(Location(val));
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

static void trim_lines(vector<string>& args)
{
	for (size_t j = 0; j < args.size(); j++)
		if (args[j][args[j].size() - 1] == ';')
			args[j] = ft::trim_char(args[j], ';');
}

static void	parse_location_block(vector<string>& args, vector<string>& arr, vector<string>& tokens, size_t& i)
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
	size_t 				i;
	string 				s;

	for (i = index; i < arr.size(); i++)
	{
		s = arr[i];
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
