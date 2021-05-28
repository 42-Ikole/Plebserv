#include <header.hpp>
#include <iostream>
#include <exception>
#include <map>
#include <ctime>

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
			_extension = _path.substr(_path.find_last_of('.'), string::npos);
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

Header::Header(vector<string> in): _end_header(true)
{
	if (in.size() == 0)
		throw Plebception(ERR_INVALID_VECTOR, "empty", "");
	Parse_request(in[0]);
	in.erase(in.begin());
	in.shrink_to_fit();
	load_headers(in);
}

string	Header::content_type_switch()
{
	if (_extension == ".html")
		return ("text/html; charset=utf-8");
	if (_extension == ".css")
		return ("text/css; charset=utf-8");
	if (_extension == ".jpg")
		return ("image/jpeg");
	if (_extension == ".js")
		return ("text/javascript; charset=utf-8");
	if (_extension == ".png")
		return ("image/png");
	if (_extension == ".json")
		return ("application/json; charset=utf-8");
	if (_extension == ".svg")
		return ("image/svg+xml");
	return ("text/html; charset=utf-8");
}

void	Header::add_to_header(string s)
{
	_header_to_add += s + "\r\n";
}

inline string create_date()
{
	time_t rawtime;
	struct tm * timeinfo;
	char buf[100];

	time (&rawtime);
	timeinfo = gmtime(&rawtime);

	strftime(buf, 100, "%a, %d %b %G %H:%M:%S GMT", timeinfo);
	return (string(buf));
}

string Header::create_header(int response_code, int body_length, map<int, string> &status_text)
{
	string res = "HTTP/1.1 " + to_string(response_code) + " " + status_text[response_code] +"\r\n" + \
	"Date: " + create_date() +"\r\n" + \
	"Server: Plebserv/1.3.29 (Unix)\r\n" + \
	"Content-Language: nl\r\n" + \
	"Connection: keep-alive\r\n" + \
	"Content-Type: " + content_type_switch() + "\r\n" + \
	"Content-Length: " + to_string(body_length) + "\r\n";

	res += _header_to_add;

	if (_end_header)
		res += "\r\n";
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
