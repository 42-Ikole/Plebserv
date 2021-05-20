#include <fcntl.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

#include <server.hpp>
#include <utilities.hpp>
#include <plebception.hpp>

using namespace std;
#define BUFFER 128

vector<string> get_lines(string file)
{
	int 			fd	= open(file.c_str(), O_RDONLY);
	char			incoming[BUFFER + 1];
	string			str_buff = "";
	vector<string>	res;

	if (fd == -1)
		throw Plebception(ERR_FD, "FD", file);
	for (int ret = 1; ret > 0;)
	{
		ret = read(fd, incoming, BUFFER);
		if (ret < 1)
			break ;
		incoming[ret] = '\0';
		str_buff += incoming;
		for (size_t pos = str_buff.find("\n"); pos != string::npos; pos = str_buff.find("\n"))
		{
			res.push_back(str_buff.substr(0, pos));
			str_buff = str_buff.substr(pos + 1, str_buff.size());
		}
	}
	res.push_back(str_buff.substr(0, str_buff.size()));
	return (res);
}

vector<Server>	get_servers(vector<string> lines)
{
	vector<Server>	res;
	int				depth;
	
	for (vector<string>::iterator it = lines.begin(); it != lines.end();)
	{
		depth = 1;
		string s = ft::ltrim(*it);
		if (s == "server {")
		{
			vector<string>::iterator start = it++;
			while (depth != 0)
			{
				s = ft::ltrim(*it);
				if (s.find("{") != string::npos)
					depth++;
				else if (s.find("}") != string::npos)
					depth--;
				it++;
			}
			res.push_back(Server(vector<string>(start, it)));
		}
		else
			it++;
	}
	return (res);
}

void	depth_check(vector<string> lines, string filename)
{
	int depth = 0;
	for (size_t i = 0; i < lines.size(); i++)
	{
		depth += count(lines[i].begin(), lines[i].end(), '{');
		depth -= count(lines[i].begin(), lines[i].end(), '}');
	}
	if (depth != 0)
		throw Plebception(ERR_BLOCK, filename, "go to dentist, missing brackets: " + to_string(depth));
}

/*
	Loading +
	Checking if valid
	Parsing into servers
*/
vector<Server> load_config(string filename)
{
	vector<string> lines = get_lines(filename);
	depth_check(lines, filename);
	vector<Server> res = get_servers(lines);

	return (res);
}
