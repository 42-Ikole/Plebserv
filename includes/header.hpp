#ifndef HEADER_HPP
#define HEADER_HPP

#include <string>
#include <vector>
#include <plebception.hpp>
#include <utilities.hpp>
#include <iomanip>
#include <map>


using namespace std;

class Header
{
public:
	string	_method;
	string	_path;
	string	_query;
	string	_extension;
	string	_http_version;
	string	_header_to_add;
	bool	_end_header;
	map<string, string> _headers;
public:
	Header(vector<std::string> in);
	// virtual ~Header();

	void	Parse_request(string request);
	string	content_type_switch();
	string	create_header(int response_code, int body_length, map<int, string> &status_text);
	void	add_to_header(string s);
	void	load_headers(vector<string> in);
};

std::ostream &operator<<(std::ostream &out, Header const &value);

#endif
