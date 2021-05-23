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

	string cgi_response(const Header &h, size_t &len);
	string read_response(const Header &h, char **env, size_t &len);
};

std::ostream &operator<<(std::ostream &out, Cgi const &value);

#endif