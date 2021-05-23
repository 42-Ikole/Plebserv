#include <server.hpp>
#include <fcntl.h>
#include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
#include <string.h>
#include <map>
#include <dirent.h>

/*
	- de path matchen met een location
	- de file readen
	- response header maken
*/

map<int, string> create_map()
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

#define ERROR_PAGE "html/error_page/error.html"

static string err_default = "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Plebbin reeee</title></head><body style='background-color: #f72d49; padding: 50px 10vw 0 10vw; color: #3f3f3f;'><h1>Error: $error_code</h1><p style='size: 15px;'>$error_message</p></body></html>";

void	Server::err_code_file(vector<unsigned char> &body, int response_code)
{
	int fd;
	int ret = 1;
	char buf[1025];
	size_t i = 0;

	std::cout << "Error pages size: "<< _error_pages.size() << std::endl;
	if (_error_pages[response_code].empty())
	{
		string to_push = err_default;
		to_push.replace(to_push.find("$error_code"), 11, to_string(response_code));
		to_push.replace(to_push.find("$error_message"), 14, g_http_errors[response_code]);
		body.resize(to_push.length());
		memcpy(&body[0], to_push.c_str(), to_push.length());
	}
	else
	{
		if ((fd = open(_error_pages[response_code].c_str(), O_RDONLY)) == -1)
			throw Plebception(ERR_FD, "err_read_file", ERROR_PAGE);
		while (ret)
		{
			ret = read(fd, &buf, 1024);
			if (ret < 0)
				throw Plebception(ERR_READ, "err_read_file", ERROR_PAGE);
			if (ret == 0)
				break ;
			buf[ret] = '\0';
			body.resize(body.size() + ret);
			memcpy(&body[i], buf, ret);
			i += ret;
		}
	}
}

void inline create_dirlist(string root, string path, vector<unsigned char> &body)
{
	string res = "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><title>Directory listing of $DIR </title></head><body><h1>Directory listing of $DIR</h1><br><br>";
	DIR *dir;
	struct dirent *cur_file;
	dir = opendir(string((root + path)).c_str());

	res.replace(res.find("$DIR"), 4, path);
	res.replace(res.find("$DIR"), 4, path);
	if (dir != NULL)
	{
		while ((cur_file = readdir(dir)))
		{
			if (cur_file->d_type == DT_DIR)
			{
				// icoontje idk idc idgaf
				res += "<a href='" + string(cur_file->d_name) + "/'>" + string(cur_file->d_name) + "</a> <br>";
			}
			else
				res += "<a href='" + string(cur_file->d_name) + "'>" + string(cur_file->d_name) + "</a> <br>";
		}
		(void) closedir (dir);
	}
	res += "</body></html>";
	body.resize(res.length());
	memcpy(&body[0], res.c_str(), res.length());
}

void	read_file(vector<unsigned char> &rv, string path)
{
	int fd;
	int ret = 1;
	char buf[1025];
	size_t i = 0;

	if ((fd = open(path.c_str(), O_RDONLY)) == -1)
		throw Plebception(ERR_FD, "read_file", path);
	while (ret)
	{
		ret = read(fd, &buf, 1024);
		if (ret < 0)
			throw Plebception(ERR_READ, "read_file", path);
		buf[ret] = '\0';
		rv.resize(rv.size() + ret);
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

vector<unsigned char>	Server::create_response(Header h, size_t *len)
{
	vector<unsigned char> rval;
	vector<unsigned char> body;
	string header;
	int response_code = 200;
	Location *l = match_location(h._path);
	if (l == NULL)
		throw Plebception(ERR_NO_LOCATION, "create_response", h._path);

	try
	{
		string file_path = l->find_file(h, response_code);
		if (l->needs_cgi(h, file_path))
			l->run_cgi(h, body, file_path);	
		else
			read_file(body, file_path);
	}
	catch(const std::exception& e)
	{
		std::cout << "ENDS WITH: " << ft::ends_with(h._path, "/") << " AUTOINDEX " << l->_auto_index << endl;
		if (response_code == 404 && ft::ends_with(h._path, "/") && l->_auto_index == on)
		{
			response_code = 200;
			create_dirlist(l->_root, h._path, body);
		}
		else
		{
			std::cerr << e.what() << " response_code: " << response_code << '\n';
			err_code_file(body, response_code);
		}
	}
	header = h.create_header(response_code, body.size(), g_http_errors);

	std::cout << "BODY SIZE: " << body.size() << " HEADER " << header.length() << std::endl;

	// creating return value
	rval.resize(header.length() + body.size());
	memcpy(&rval[0], header.c_str(), header.length());
	memcpy(&rval[header.length()], &body[0], body.size());
	return (rval);
}
