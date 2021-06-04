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
	bool	_chonky;
	map<string, string> _headers_in;
	map<string, string> _headers_out;
public:
	Header();
	Header(vector<std::string> in);
	Header &operator=(const Header &h);
	Header(const Header &h);
	// virtual ~Header();

	void	Parse_request(string request);
	string	content_type_switch();
	string	create_header(int response_code, int body_length, map<int, string> &status_text);
	void	load_headers_in(vector<string> in);
	void	add_to_header_out(string val, string key);
	void	add_to_header_out(vector<string> head);

};

std::ostream &operator<<(std::ostream &out, Header const &value);
string create_date();

#endif
