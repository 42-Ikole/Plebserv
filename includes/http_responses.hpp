
#ifndef HTTP_RESPONSES_HPP
# define HTTP_RESPONSES_HPP

# include <string>
# include <map>

using namespace std;

static map<int, string> create_http_map()
{
	map<int, string> m;
/*
**	Informational
*/
	m[100] = "Continue";
	m[101] = "Switching Protocols";
	m[102] = "Processing (WebDAV)";

/*
**	SUCCESS
*/
	m[200] = "OK";
	m[201] = "Created";
	m[202] = "Accepted";
	m[203] = "Non-Authoritative Information";
	m[204] = "No Content";
	m[205] = "Reset Content";
	m[206] = "Partial Content";
	m[207] = "Multi-Status";

/*
**	Redirection
*/
	m[300] = "Multiple Choices";
	m[301] = "Moved Permanently";
	m[302] = "Found";
	m[303] = "See Other";
	m[304] = "Not Modified";
	m[305] = "Use Proxy";
	m[306] = "(Unused)";
	m[307] = "Temporary Redirect";
	m[308] = "Permanent Redirect (experimental)";

/*
**	Client error
*/
	m[400] = "Bad Request";
	m[401] = "Unauthorized";
	m[402] = "Payment Required";
	m[403] = "Forbidden";
	m[404] = "Not Found";
	m[405] = "Method Not Allowed";
	m[406] = "Not Acceptable";
	m[407] = "Proxy Authentication Required";
	m[408] = "Request Timeout";
	m[409] = "Conflict";
	m[410] = "Gone";
	m[411] = "Length Required";
	m[412] = "Precondition Failed";
	m[413] = "Request Entity Too Large";
	m[414] = "Request-URI Too Long";
	m[415] = "Unsupported Media Type";
	m[416] = "Requested Range Not Satisfiable";
	m[417] = "Expectation Failed";
	m[418] = "I'm a teapot";
	m[419] = "Page expired";
	m[420] = "Spark up baby / enhance your calm";
	m[422] = "Unprocessable Entity";
	m[423] = "Locked";
	m[424] = "Failed Dependency";
	m[426] = "Upgrade Required";
	m[428] = "Precondition Required";
	m[429] = "Too Many Requests";
	m[431] = "Request Header Fields Too Large";
	m[450] = "Blocked by Windows Parental Control";
	m[451] = "Unavailable For Legal Reasons";
	m[494] = "Request Header Too Large";
	m[495] = "Cert Error";
	m[496] = "No Cert";
	m[497] = "HTTP to HTTPS";
	m[498] = "Token expired/invalid";
	m[499] = "Token required";

/*
** Server error
*/
	m[500] = "Internal Server Error";
	m[501] = "Not Implemented";
	m[502] = "Bad Gateway";
	m[503] = "Service Unavailable";
	m[504] = "Gateway Timeout";
	m[505] = "HTTP Version Not Supported";
	m[509] = "Bandwidth Limit Exceeded";
	m[510] = "Not Extended";
	m[511] = "Network Authentication Required";
	m[522] = "Network read timeout error";
	m[525] = "Network connect timeout error";
	return m;
}

static map<int, string> g_http_errors = create_http_map();

#endif
