#include <server.hpp>
#include <fcntl.h>
#include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
#include <string.h>
#include <map>
#include <dirent.h>
#include <string>

/*
	- de path matchen met een location
	- de file readen
	- response header maken
*/

#define HYPERLINK_OPEN	"<a href='"
#define HYPERLINK_CLOSE	"</a> <br>"

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

static string err_default = "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Plebbin reeee</title></head><body style='background-color: #f72d49; padding: 50px 10vw 0 10vw; color: #3f3f3f;'><h1>Error: $error_code</h1><p style='size: 15px;'>$error_message</p></body></html>";

static void		default_error_page(string &body, int response_code)
{
	string to_push = err_default;
	to_push.replace(to_push.find("$error_code"), 11, ft::to_string(response_code));
	to_push.replace(to_push.find("$error_message"), 14, g_http_errors[response_code]);
	ft::str_set(body, to_push);
}

void	Server::err_code_file(string &body, int response_code)
{
	int fd;
	int ret = 1;
	char buf[1025];

	if (_error_pages[response_code].empty())
		default_error_page(body, response_code);
	else
	{
		try
		{
			if ((fd = open(_error_pages[response_code].c_str(), O_RDONLY)) == -1)
				throw Plebception(ERR_FD, "err_read_file", _error_pages[response_code]);
		}
		catch (Plebception &msg)
		{
			cerr << msg.what() << " Falling back to server default!" << endl;
			default_error_page(body, response_code);
			return ;
		}
		for (size_t i = 0; ret;)
		{
			ret = read(fd, &buf, 1024);
			if (ret < 0)
				throw Plebception(ERR_READ, "err_read_file", _error_pages[response_code]);
			if (ret == 0)
				break ;
			buf[ret] = '\0';
			body.resize(body.size() + ret);
			memcpy(&body[i], buf, ret);
			i += ret;
		}
	}
}

void inline create_dirlist(string root, string path, string &body)
{
	string res = "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><title>Directory listing of $DIR </title></head><body><h1>Directory listing of $DIR</h1><br><br>";
	DIR *dir;
	struct dirent *cur_file;
	
	dir = opendir(string((root + path)).c_str());
	for (size_t pos = res.find("$DIR"); pos != string::npos; pos = res.find("$DIR"))
		res.replace(pos, 4, path);
	if (dir != NULL)
	{
		while ((cur_file = readdir(dir)))
		{
			if (cur_file->d_type == DT_DIR)
				res += HYPERLINK_OPEN + string(cur_file->d_name) + "/'>" + string(cur_file->d_name) + HYPERLINK_CLOSE;
			else
				res += HYPERLINK_OPEN + string(cur_file->d_name) + "'>" + string(cur_file->d_name) + HYPERLINK_CLOSE;
		}
		(void)closedir(dir);
	}
	res += "</body></html>";
	ft::str_set(body, res);
}

void	read_file(string &rv, string path)
{
	int fd;
	int ret = 1;
	char buf[1025];

	if ((fd = open(path.c_str(), O_RDONLY)) == -1)
		throw Plebception(ERR_FD, "read_file", path);
	for (size_t i = 0; ret;)
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
			if (!closest_match || _locations[i]._location.length() > closest_match->_location.length())
				closest_match = &_locations[i];
	}
	return (closest_match);
}

string	Server::return_get(Header &h, Location *l)
{
	int response_code = 200;
	size_t body_size = 0;
	string body;

	try
	{
		string file_path = l->find_file(h, response_code);
		if (!l->run_cgi(h, body, file_path, *this, body_size))
			read_file(body, file_path);
	}
	catch(const std::exception& e)
	{
		std::cout << "ENDS WITH: " << ft::ends_with(h._path, "/") << " AUTOINDEX " << l->_auto_index << endl;
		if (response_code == 404 && ft::ends_with(h._path, "/") && l->_auto_index == ON)
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
	return (h.create_header(response_code, body.size(), g_http_errors) + string(body));
}

/*
	POST

	if path == cgi
		run cgi && return
	if path == dir
		store data in location or this dir
	if path == file
		revert to get request

*/

string	Server::return_post(Header &h, Location *l, string &body)
{
	int response_code = 200;
	size_t body_size = 0;

	try
	{
		string file_path = l->find_file(h, response_code);
		if (!l->run_cgi(h, body, file_path, *this, body_size))
			return (return_get(h, l));
	}
	catch (const std::exception& e)
	{
		int fd = 0;
		string full_path = l->_root + "/" + h._path.replace(h._path.find(l->_location), l->_location.size(), "");
		struct stat file_status;
		if (stat(full_path.c_str(), &file_status) == -1 || file_status.st_mode & S_IFREG)
			fd = open(full_path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
		else if (file_status.st_mode & S_IFDIR)
			fd = open(string(full_path + "/" + create_date()).c_str(), O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
		if (fd == -1)
			throw Plebception(ERR_FD, "return_post", full_path);
		write(fd, body.c_str(), body.length());
		close(fd);
		response_code = 201;
	}
	return (h.create_header(response_code, body.size(), g_http_errors) + string(body));
}

string	Server::return_delete(Header &h, Location *l)
{
	int response_code = 204;
	string fullpath = l->_root + h._path;

	if (unlink(fullpath.c_str()) == -1)
		response_code = 403;
	return (h.create_header(response_code, 0, g_http_errors));
}

string	Server::return_put(Header &h, Location *l, string &body)
{
	string fullpath = l->_upload_store + "/" + h._path.replace(h._path.find(l->_location), l->_location.size(), "");
	struct stat file_status;
	int response_code = 201;
	int fd;

	cout << "Path to save to: " << fullpath << endl;

	if (stat(fullpath.c_str(), &file_status) == -1)
	{
		fd = open(fullpath.c_str(), O_CREAT | O_WRONLY, 0777);
		write(fd, body.c_str(), body.size());
	}
	else
	{
		fd = open(fullpath.c_str(), O_TRUNC | O_WRONLY, 0777);
		write(fd, body.c_str(), body.size());
		response_code = 204;
	}
	return(h.create_header(response_code, 0, g_http_errors));
}


string	Server::return_options(Header &h, Location *l)
{
	string header;
	string allowed;

	if (l->_limit_except.size() == 0)
		allowed = "OPTIONS, GET, HEAD, POST, PUT, DELETE, CONNECT, TRACE";
	else
		for (size_t i = 0; i < l->_limit_except.size(); i++)
		{
			if (i + 1 == l->_limit_except.size())
				allowed += l->_limit_except[i];
			else
				allowed += l->_limit_except[i] + ", ";
		}
	std::cout << "Allowed: " << allowed << endl;
	h.add_to_header_out("Allow", allowed);
	header = h.create_header(204, 0, g_http_errors);
	return (header);
}

string	Server::return_head(Header &h, Location *l)
{
	string body;
	size_t body_size = 0;
	int response_code = 200;

	try
	{
		string file_path = l->find_file(h, response_code);
		if (!l->run_cgi(h, body, file_path, *this, body_size))
			read_file(body, file_path);
	}
	catch(const exception& e)
	{
		// std::cout << "ENDS WITH: " << ft::ends_with(h._path, "/") << " AUTOINDEX " << l->_auto_index << endl;
		if (response_code == 404 && ft::ends_with(h._path, "/") && l->_auto_index == ON)
		{
			response_code = 200;
			create_dirlist(l->_root, h._path, body);
		}
		else
		{
			cerr << e.what() << " response_code: " << response_code << '\n';
			err_code_file(body, response_code);
		}
	}
	body_size = body.size();
	return (h.create_header(response_code, body_size, g_http_errors));
}

string	Server::create_response(Header &h, string &body)
{
	Location *l = match_location(h._path);
	if (l == NULL)
		throw Plebception(ERR_NO_LOCATION, "create_response", h._path);
	std::cout << "The match is " << l->_location << std::endl;
	if (h._method == "GET")
		return (return_get(h, l));
	if (h._method == "POST")
		return (return_post(h, l, body));
	if (h._method == "DELETE")
		return (return_delete(h, l));
	if (h._method == "PUT")
		return (return_put(h, l, body));
	if (h._method == "OPTIONS")
		return (return_options(h, l));
	if (h._method == "HEAD")
		return (return_head(h, l));
	return (return_post(h, l, body));
}
