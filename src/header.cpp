#include <header.hpp>
#include <iostream>
#include <exception>
#include <map>

using namespace std;

void	Header::Parse_request(string request)
{
	vector<string> parsed = ft::split(request);

	try
	{
		_method = parsed[0].substr(0, parsed[0].find('?'));

		if (parsed[1].find("?") != string::npos)
		{
			vector<string> tmp = ft::split(parsed[1], "?");
			_path = tmp[0];
			_query = tmp[1];
		}
		else
			_path = parsed[1];
		if (_path.find('.') != string::npos)
			_extension = _path.substr(_path.find('.'), string::npos);
		_http_version = parsed[2];
	}
	catch (exception &e)
	{
		cout << e.what() << endl;
		throw Plebception(ERR_INVALID_VALUE, "parsing request", "");
	}
}

void	Header::load_headers(vector<string> in)
{
	for (size_t i = 0; i < in.size() - 1; i++)
		_headers[in[i].substr(0, in[i].find(':'))] = in[i].substr(in[i].find(':') + 2, string::npos);
}

Header::Header(vector<string> in)
{
	if (in.size() == 0)
		throw Plebception(ERR_INVALID_VECTOR, "empty", "");
	Parse_request(in[0]);
	in.erase(in.begin());

	load_headers(in);
}

string	Header::content_type_switch()
{
	if (_extension == ".html")
		return ("text/html");
	if (_extension == ".css")
		return ("text/css");
	if (_extension == ".jpg")
		return ("image/jpeg");
	if (_extension == ".js")
		return ("text/javascript");
	if (_extension == ".png")
		return ("image/png");
	if (_extension == ".json")
		return ("application/json");
	return ("text/html");
}

string Header::create_header(int response_code, int body_length, map<int, string> &status_text)
{
	string res = "HTTP/1.1 " + to_string(response_code) + status_text[response_code] +"\n" + \
	"Date: Thu, 08 Apr 2004 18:24:33 GMT\n" + \
	"Server: Plebserv/1.3.29 (Unix) PHP/4.3.4 X-Powered-By: PHP/4.3.4\n" + \
	"Content-Language: nl\n" + \
	"charset=iso-8859-1\n" + \
	"Connection: close\n" + \
	"Content-Type: " + content_type_switch() + "\n" + \
	"Content-Length: " + to_string(body_length) + "\n\r\n";

	return (res);
}

std::ostream &operator<<(std::ostream &out, Header const &value)
{
	out << "------------ HEADER --------" << std::endl;

	out << std::setw(15) << "REQUEST LINE | " << "[" << value._method << "] [" << value._path << "]" << std::endl;
	out << std::setw(15) << "QUERY | " << value._query << std::endl;
	out << std::setw(15) << "EXTENSION | " << value._extension << std::endl;
	out << std::setw(15) << "OTHER HEADERS:\n";
	for (map<string, string>::const_iterator i = value._headers.begin(); i != value._headers.end(); i++)
		out << i->first << ": " <<  i->second << std::endl;

	out << "------------ DONE ----------" << std::endl;
	return (out);
}
