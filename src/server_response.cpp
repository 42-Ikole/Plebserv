/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*	 																				 */
/*     _ (`-.              ('-. .-. .-')    .-')      ('-.  _  .-')        (`-.   	 */
/*    ( (OO  )           _(  OO)\  ( OO )  ( OO ).  _(  OO)( \( -O )     _(OO  )_ 	 */
/*   _.`     \ ,--.     (,------.;-----.\ (_)---\_)(,------.,------. ,--(_/   ,. \	 */
/*  (__...--'' |  |.-')  |  .---'| .-.  | /    _ |  |  .---'|   /`. '\   \   /(__/	 */
/*   |  /  | | |  | OO ) |  |    | '-' /_)\  :` `.  |  |    |  /  | | \   \ /   / 	 */
/*   |  |_.' | |  |`-' |(|  '--. | .-. `.  '..`''.)(|  '--. |  |_.' |  \   '   /, 	 */
/*   |  .___.'(|  '---.' |  .--' | |  \  |.-._)   \ |  .--' |  .  '.'   \     /__)	 */
/*   |  |      |      |  |  `---.| '--'  /\       / |  `---.|  |\  \     \   /    	 */
/*   `--'      `------'  `------'`------'  `-----'  `------'`--' '--'     `-'     	 */
/*																					 */
/* 									MADE BY											 */
/* 		—————————————————————————————————————————————————————————————————————		 */
/*				 Alpha_1337k       |    https://github.com/Alpha1337k				 */
/*				 VictorTennekes    |    https://github.com/VictorTennekes			 */
/*				 Kingmar	 	   |    https://github.com/K1ngmar					 */
/*																					 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <map>
#include <dirent.h>
#include <string>

#include <server.hpp>
#include <http_responses.hpp>

#define HYPERLINK_OPEN	"<a href='"
#define HYPERLINK_CLOSE	"</a> <br>"

static string err_default = "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Plebbin reeee</title></head><body style='background-color: #f72d49; padding: 50px 10vw 0 10vw; color: #3f3f3f;'><h1>Error: $error_code</h1><p style='size: 15px;'>$error_message</p></body></html>";

static void		default_error_page(string& body, int response_code)
{
	string to_push = err_default;
	to_push.replace(to_push.find("$error_code"), 11, ft::to_string(response_code));
	to_push.replace(to_push.find("$error_message"), 14, g_http_errors[response_code]);
	ft::str_set(body, to_push);
}

void	Server::err_code_file(string& body, int response_code)
{
	int		fd;
	int		ret = 1;
	char	buf[1025];

	if (error_pages[response_code].empty())
		default_error_page(body, response_code);
	else
	{
		try
		{
			if ((fd = open(error_pages[response_code].c_str(), O_RDONLY)) == -1)
				throw Plebception(ERR_FD, "err_read_file", error_pages[response_code]);
		}
		catch (Plebception& msg)
		{
			cerr << msg.what() << " Falling back to server default!" << endl;
			default_error_page(body, response_code);
			return ;
		}
		for (size_t i = 0; ret;)
		{
			ret = read(fd, &buf, 1024);
			if (ret < 0)
				throw Plebception(ERR_READ, "err_read_file", error_pages[response_code]);
			if (ret == 0)
				break ;
			buf[ret] = '\0';
			body.resize(body.size() + ret);
			memcpy(&body[i], buf, ret);
			i += ret;
		}
	}
}

static void inline create_dirlist(string root, string path, string& body)
{
	string			res = "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><title>Directory listing of $DIR </title></head><body><h1>Directory listing of $DIR</h1><br><br>";
	DIR*			dir;
	struct dirent*	cur_file;
	
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

void	Location::read_file(string&  rv, string path)
{
	int		fd;
	int		ret = 1;
	char	buf[1025];

	if (static_dir && static_files[path].empty() == false)
	{
		cout << "using cached response: " << path << endl;
		rv = static_files[path];
		return;
	}
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
	if (static_dir)
		static_files[path] = rv;
	close (fd);
}

Location*	Server::match_location(string path)
{
	Location *closest_match = 0;

	for (size_t i = 0; i < locations.size(); i++)
	{
		// std::cout << "Matching [" << locations[i].location << "] with [" << path << "]\n";
		if (!strncmp(locations[i].location.c_str(), path.c_str(), locations[i].location.length()))
			if (!closest_match || locations[i].location.length() > closest_match->location.length())
				closest_match = &locations[i];
	}
	return (closest_match);
}

string	Server::return_get(Header& h, Location* l)
{
	int		response_code = 200;
	string	body;

	try
	{
		string file_path = l->find_file(h, response_code);
		if (!l->run_cgi(h, body, file_path, *this))
			l->read_file(body, file_path);
	}
	catch(const std::exception& e)
	{
		//std::// cout << "ENDS WITH: " << ft::ends_with(h._path, "/") << " AUTOINDEX " << l->auto_index << endl;
		if (response_code == 404 && ft::ends_with(h._path, "/") && l->auto_index == ON)
		{
			response_code = 200;
			create_dirlist(l->root, h._path, body);
		}
		else
		{
			std::cerr << e.what() << " response_code: " << response_code << '\n';
			err_code_file(body, response_code);
			h._extension = ".html";
		}
	}
	return (h.create_header(response_code, body.size()) + string(body));
}

string	Server::return_post(Header& h, Location* l, string& body)
{
	int		response_code = 200;

	try
	{
		if (body.size() > l->max_body_size)
		{
			response_code = 413;
			body.clear();
			err_code_file(body, response_code);
			return (h.create_header(response_code, body.size()) + string(body));
		}
		string file_path = l->find_file(h, response_code);
		if (!l->run_cgi(h, body, file_path, *this))
			return (return_get(h, l));
	}
	catch (const std::exception& e)
	{
		if (response_code != 404)
		{
			std::cerr << e.what() << " response_code: " << response_code << '\n';
			err_code_file(body, response_code);
		}
		else
		{
			int fd = 0;
			string full_path = l->root + "/" + h._path.replace(h._path.find(l->location), l->location.size(), "");
			struct stat file_status;

			if (stat(full_path.c_str(), &file_status) == -1 || file_status.st_mode & S_IFREG)
				fd = open(full_path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
			else if (file_status.st_mode & S_IFDIR)
				fd = open(string(full_path + "/" + ft::create_date()).c_str(), O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
			if (fd == -1)
				throw Plebception(ERR_FD, "return_post", full_path);
			write(fd, body.c_str(), body.length());
			close(fd);
			response_code = 201;
		}
	}
	return (h.create_header(response_code, body.size()) + string(body));
}

string	Server::return_delete(Header& h, Location* l)
{
	int		response_code = 204;
	string	full_path = l->root + h._path;

	if (unlink(full_path.c_str()) == -1)
		response_code = 403;
	return (h.create_header(response_code, 0));
}

string	Server::return_put(Header& h, Location* l, string& body)
{
	string		full_path = l->upload_store + "/" + h._path.replace(h._path.find(l->location), l->location.size(), "");
	struct stat	file_status;
	int			response_code = 201;
	int			fd;

	// cout << "Path to save to: " << full_path << endl;
	if (body.size() > l->max_body_size)
	{
		response_code = 413;
		body.clear();
		err_code_file(body, response_code);
		return (h.create_header(response_code, body.size()) + string(body));
	}
	if (stat(full_path.c_str(), &file_status) == -1)
		fd = open(full_path.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0777);
	else
	{
		fd = open(full_path.c_str(), O_TRUNC | O_RDWR, 0777);
		response_code = 204;
	}
	if (fd == -1)
		throw Plebception(ERR_FD, "opening file failed", full_path);
	else
	{
		int	ret = 1;
		for (size_t i = 0; ret > 0; i += ret)
		{
			ret = write(fd, &body[i], body.size() - i);
			if (ret == -1)
				throw Plebception(ERR_WRITING, "writing to file", full_path);
		}
	}
	close(fd);
	return(h.create_header(response_code, 0));
}


string	Server::return_options(Header& h, Location* l)
{
	string header;
	string allowed;

	if (l->limit_except.size() == 0)
		allowed = "OPTIONS, GET, HEAD, POST, PUT, DELETE, CONNECT, TRACE";
	else
		for (size_t i = 0; i < l->limit_except.size(); i++)
		{
			if (i + 1 == l->limit_except.size())
				allowed += l->limit_except[i];
			else
				allowed += l->limit_except[i] + ", ";
		}
	//std::cout << "Allowed: " << allowed << endl;
	h.add_to_header_out("Allow", allowed);
	header = h.create_header(204, 0);
	return (header);
}

string	Server::return_head(Header& h, Location* l)
{
	string 	body;
	int		response_code = 200;

	try
	{
		string file_path = l->find_file(h, response_code);
		if (!l->run_cgi(h, body, file_path, *this))
			l->read_file(body, file_path);
	}
	catch(const exception& e)
	{
		if (response_code == 404 && ft::ends_with(h._path, "/") && l->auto_index == ON)
		{
			response_code = 200;
			create_dirlist(l->root, h._path, body);
		}
		else
		{
			cerr << e.what() << " response_code: " << response_code << '\n';
			err_code_file(body, response_code);
		}
	}
	return (h.create_header(response_code, body.size()));
}

string	Server::create_response(Header& h, string& body)
{
	int			response_code = 200;
	Location*	l = match_location(h._path);

	if (l == NULL)
		throw Plebception(ERR_NO_LOCATION, "create_response", h._path);
	try {l->method_allowed(h, response_code); }
	catch (std::exception& e)
	{
		cerr << e.what() << response_code << endl;
		return (h.create_header(response_code, 0));
	}
	if (l->redir.first != 0)
	{
		h.add_to_header_out("Location", l->redir.second);
		return (h.create_header(l->redir.first, 0));
	}
	//std::cout << "The match is " << l->location << std::endl;
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
