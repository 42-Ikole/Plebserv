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
