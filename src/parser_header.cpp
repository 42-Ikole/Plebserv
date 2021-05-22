#include <header.hpp>
#include <iostream>
#include <exception>

using namespace std;

void	Header::Parse_request(string request)
{
	vector<string> parsed = ft::split(request);

	try
	{	
		_method	= parsed[0];
		_path	= parsed[1];
		_http_version = parsed[2];
	}
	catch (exception &e)
	{
		cout << e.what() << endl;
		throw Plebception(ERR_INVALID_VALUE, "parsing request", "");
	}
}

Header::Header(vector<string> in)
{
	Parse_request(in[0]);
	in.erase(in.begin());
	_other_headers = in;
}

// HTTP/1.1 200 OK
// Date: Thu, 08 Apr 2004 18:24:33 GMT
// Server: Apache/1.3.29 (Unix) PHP/4.3.4 X-Powered-By: PHP/4.3.4
// Content-Language: nl
// Content-Type: text/html;
// charset=iso-8859-1
// X-Cache: MISS from wikipedia.org
// Connection: close 
// Content-Type: text/html
// Content-Length: 49"

inline bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

string	Header::content_type_switch()
{
	if (ends_with(_path, ".html"))
		return ("text/html");
	if (ends_with(_path, ".css"))
		return ("text/css");
	if (ends_with(_path, ".jpg"))
		return ("image/jpeg");
	if (ends_with(_path, ".js"))
		return ("text/javascript");
	if (ends_with(_path, ".png"))
		return ("image/png");
	return ("text/html");
}

string Header::create_header(int response_code, int body_length)
{
	string res = "HTTP/1.1 " + to_string(response_code) + " OK\n" + \
	"Date: Thu, 08 Apr 2004 18:24:33 GMT\n" + \
	"Server: Plebserv/1.3.29 (Unix) PHP/4.3.4 X-Powered-By: PHP/4.3.4\n" + \
	"Content-Language: nl\n" + \
	"charset=iso-8859-1\n" + \
	"X-Cache: MISS from wikipedia.org\n" + \
	"Connection: close\n" + \
	"Content-Type: " + content_type_switch() + "\n" + \
	"Content-Length: " + to_string(body_length) + "\n";

	return (res);
}

std::ostream &operator<<(std::ostream &out, Header const &value)
{
	out << "------------ HEADER --------" << std::endl;

	out << std::setw(15) << "REQUEST LINE | " << "[" << value._method << "] [" << value._path << "]" << std::endl;
	/*
		implemented headers
	*/
	
	out << std::setw(15) << "OTHER HEADERS:\n";
	for (size_t i = 0; i < value._other_headers.size(); i++)
		out << value._other_headers[i] << std::endl;

	out << "------------ DONE ----------" << std::endl;
	return (out);
}

// /*



// GET /buy.html HTTP/1.1
// Host: localhost:8080
// Connection: keep-alive
// Pragma: no-cache
// Cache-Control: no-cache
// sec-ch-ua: " Not A;Brand";v="99", "Chromium";v="90", "Google Chrome";v="90"
// sec-ch-ua-mobile: ?0
// DNT: 1
// Upgrade-Insecure-Requests: 1
// User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.212 Safari/537.36
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9
// Sec-Fetch-Site: none
// Sec-Fetch-Mode: navigate
// Sec-Fetch-User: ?1
// Sec-Fetch-Dest: document
// Accept-Encoding: gzip, deflate, br
// Accept-Language: nl-NL,nl;q=0.9,ru-RU;q=0.8,ru;q=0.7,en-US;q=0.6,en;q=0.5


// HTTP/1.1 200 OK
// Date: Thu, 08 Apr 2004 18:24:33 GMT
// Server: Apache/1.3.29 (Unix) PHP/4.3.4 X-Powered-By: PHP/4.3.4
// Content-Language: nl
// Content-Type: text/html;
// charset=iso-8859-1
// X-Cache: MISS from wikipedia.org
// Connection: close 
// Content-Type: text/html
// Content-Length: 49"
//
//
// data







// */