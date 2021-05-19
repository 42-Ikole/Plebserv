#include <fcntl.h>
#include <iostream>     // std::cout'
#include <string>
#include <unistd.h>
#include <vector>

#include <error.hpp>
#include <server.hpp>

using namespace std;
#define BUFFER 128

vector<string> get_lines(string file)
{
	int 			fd	= open(file.c_str(), O_RDONLY);
	char			incoming[BUFFER + 1];
	string			str_buff = "";
	vector<string>	res;

	if (fd == -1)
		error(PARSER, "could not open file");
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
	res.push_back(str_buff.substr(0, str_buff.find("\0") + 1));
	return (res);
}

static inline std::string &ltrim(std::string &s) {
	s.erase(s.begin(), s.begin() + s.find_first_not_of(" \t"));
	return s;
}

vector<Server>	get_servers(vector<string> lines)
{
	vector<Server>	res;
	int				depth = 0;
	
	for (vector<string>::iterator it = lines.begin(); it != lines.end();)
	{
		string s = *it;
		ltrim(s);
		cout << "ik ben net buiten de server" << endl;
		if (s == "server {")
		{
			cout << "ik ben in de server" << endl;
			vector<string>::iterator start = it++;
			depth++;
			while (depth != 0)
			{
				s = *it;
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

/*
	Loading +
	Checking if valid
	Parsing into servers
*/
vector<Server> load_config(string filename)
{
	vector<string> lines = get_lines(filename);
	vector<Server> res = get_servers(lines);

	return (res);
}
