#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <header.hpp>

class Server;

using namespace std;

class Cgi
{
public:
	string _full_path;
	string _match;
public:
	Cgi(string path, string match);
	~Cgi();
	Cgi(const Cgi &tbc);
	Cgi & operator=(const Cgi &tba);

	void cgi_response(Header &h, vector<unsigned char> &body, string file_path, Server &ser, size_t &size);
	void read_response(Header &h, char** env, vector<unsigned char> &body, string file_path);
	char *create_env_var(string key, string value);
	void cgi_child(int fdin[2], int fdout[2], char *args[3], char **env);
	void cgi_parent(int fdin[2], int fdout[2], pid_t id, vector<unsigned char> &body);
};

std::ostream &operator<<(std::ostream &out, Cgi const &value);

#endif