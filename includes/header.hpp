#ifndef HEADER_HPP
#define HEADER_HPP

#include <string>
#include <vector>
#include <plebception.hpp>
#include <utilities.hpp>

using namespace std;

class Header
{
public:
	string	_method;
	string	_path;
	string	_http_version;
	string	_host;
	string	_connection;
	vector<string> _other_headers;
public:
	Header(vector<std::string> in);
	void	Parse_request(string request);
	// virtual ~Header();
};

#endif
