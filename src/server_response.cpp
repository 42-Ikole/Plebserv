#include <server.hpp>
#include <fcntl.h>
#include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
#include <string.h>
#include <map>

/*
	- de path matchen met een location
	- de file readen
	- response header maken
*/

static map<int, string> create_map()
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
	m[450] = "Geblokkeerd door Windows Parental Control";
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

map<int, string> g_http_errors = create_map();

static void	err_code_file(char *rv, int response_code)
{
	int fd;
	int ret = 1;
	char buf[1025];
	char *file;
	size_t i = 0;

	if ((fd = open("html/error_page/error.html", O_RDONLY)) == -1)
		throw Plebception(ERR_FD, "read_file", "/html/error_page/error.html");
	while (ret)
	{
		ret = read(fd, &buf, 1024);
		if (ret < 0)
			throw Plebception(ERR_READ, "read_file", "/html/error_page/error.html");
		if (ret == 0)
			break ;
		buf[ret] = '\0';
		string tmp = string(buf);
		ret = ret - 8 + g_http_errors[response_code].length() - 14;
		tmp.replace(tmp.find("$error_code"), 11, to_string(response_code));
		tmp.replace(tmp.find("$error_message"), 14, g_http_errors[response_code]);
		for (size_t j = 0; j < 1025 && tmp[j]; j++)
			buf[j] = tmp[j];
		buf[ret] = '\0';
		std::cout << "ret: " << ret << "i " << i << std::endl;
		memcpy(&rv[i], buf, ret);
		i += ret;
	}
	cerr << "wazap" << endl;
}

void	read_file(char *rv, string path)
{
	int fd;
	int ret = 1;
	char buf[1025];
	char *file;
	size_t i = 0;

	if ((fd = open(path.c_str(), O_RDONLY)) == -1)
		throw Plebception(ERR_FD, "read_file", path);
	while (ret)
	{
		ret = read(fd, &buf, 1024);
		if (ret < 0)
			throw Plebception(ERR_READ, "read_file", path);
		buf[ret] = '\0';
		std::cout << "ret: " << ret << "i " << i << std::endl;
		memcpy(&rv[i], buf, ret);
		i += ret;
	}
}

Location	*Server::match_location(string path)
{
	Location *closest_match = 0;
	for (size_t i = 0; i < _locations.size(); i++)
	{
		std::cout << "Matching [" << _locations[i]._location << "] with [" << path << "]\n";
		if (!strncmp(_locations[i]._location.c_str(), path.c_str(), _locations[i]._location.length()))
			return (&_locations[i]);
		{
			if (!closest_match || _locations[i]._location.length() > closest_match->_location.length())
				closest_match = &_locations[i];
		}
	}
	//path to 404;
	return (closest_match);
}

char	*Server::create_response(Header h, size_t *len)
{
	Location *l = match_location(h._path);
	if (l == NULL)
		throw Plebception(ERR_NO_LOCATION, "create_response", h._path);
	int response_code = 200;
	size_t	file_size = 0;
	string file_path;
	try
	{
		file_path = l->find_file(h, &response_code, &file_size);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		file_size = 347 - 8 + g_http_errors[response_code].length() - 14; // niet de filesize hardcoden
		response_code = 404;
	}
	
	string response = h.create_header(response_code, file_size);

	std::cout << "response: " <<  response << "\n\nbody_size: " << file_size << " path: " << file_path << std::endl;

	/*
		Creating the return value
	*/
	char *rval = (char *)malloc(sizeof(char) * (file_size + response.length() + 3));
	rval[file_size + response.length() + 2] = 0;
	memcpy(rval, response.c_str(), response.length());
	memcpy(&rval[response.length()], "\r\n", 2);

	if (response_code == 200)
		read_file(&rval[response.length() + 2], file_path);
	else
		err_code_file(&rval[response.length() + 2], response_code);
	*len = file_size + response.length() + 2;
	return (rval);
}
