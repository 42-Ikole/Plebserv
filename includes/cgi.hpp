#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <header.hpp>

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

	void cgi_response(const Header &h, vector<unsigned char> &body, string file_path);
	void read_response(const Header &h, char **env, vector<unsigned char> &body, string file_path);
	char *create_env_var(string key, string value);
};

std::ostream &operator<<(std::ostream &out, Cgi const &value);

#endif