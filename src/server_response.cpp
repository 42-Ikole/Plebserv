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

static void		default_error_page(string& body, int response_code)
{
	string to_push(ERR_DEFAULT);
	to_push.replace(to_push.find("$error_code"), 11, ft::to_string(response_code));
	to_push.replace(to_push.find("$error_message"), 14, g_http_errors[response_code]);
	ft::str_set(body, to_push);
}

void	Server::err_code_file(string& body, int response_code)
{
	int		fd;

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
			std::cerr << msg.what() << " Falling back to server default!" << std::endl;
			default_error_page(body, response_code);
			return ;
		}
		if (ft::read(fd, body, 2048) < 0)
			throw Plebception(ERR_READ, "err_read_file", error_pages[response_code]);
	}
}

static void inline create_dirlist(string root, string path, string& body)
{
	string			res(DIR_LIST);
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

void	Location::read_file(string& rv, string path)
{
	int		fd;

	if (static_dir && static_files[path].empty() == false)
	{
		rv = static_files[path];
		return;
	}
	if ((fd = open(path.c_str(), O_RDONLY)) == -1)
		throw Plebception(ERR_FD, "read_file", path);
	if (ft::read(fd, rv, 2048) < 0)
		throw Plebception(ERR_READ, "read_file", path);
	if (static_dir)
		static_files[path] = rv;
	close (fd);
}

Location*	Server::match_location(string path)
{
	Location *closest_match = 0;

	for (size_t i = 0; i < locations.size(); i++)
	{
		if (!strncmp(locations[i].location.c_str(), path.c_str(), locations[i].location.length()))
			if (!closest_match || locations[i].location.length() > closest_match->location.length())
				closest_match = &locations[i];
	}
	return (closest_match);
}

string	Server::return_get(connect_data &data, Location* l)
{
	int		response_code = 200;
	string	body;

	try
	{
		string file_path = l->find_file(data.h, response_code);
		if (l->run_cgi(data, body, file_path, *this))
			return "";
		l->read_file(body, file_path);
	}
	catch(const Plebception& e)
	{
		if (response_code == 404 && ft::ends_with(data.h._path, "/") && l->auto_index == ON)
		{
			response_code = 200;
			create_dirlist(l->root, data.h._path, body);
		}
		else
		{
			std::cerr << e.what() << " response_code: " << response_code << std::endl;
			err_code_file(body, response_code);
			data.h._extension = ".html";
		}
	}
	return (data.h.create_header(response_code, body.size()) + string(body));
}

string	Server::return_post(connect_data &data, Location* l)
{
	int		response_code = 200;

	try
	{
		if (data.buf.size() > l->max_body_size)
		{
			response_code = 413;
			data.buf.clear();
			err_code_file(data.buf, response_code);
			return (data.h.create_header(response_code, data.buf.size()) + string(data.buf));
		}
		string file_path = l->find_file(data.h, response_code);
		if (l->run_cgi(data, data.buf, file_path, *this))
			return ("");
		return (return_get(data, l));
	}
	catch (const Plebception& e)
	{
		if (response_code != 404)
		{
			std::cerr << e.what() << " response_code: " << response_code << std::endl;
			err_code_file(data.buf, response_code);
		}
		else
		{
			int fd = -1;
			string full_path = l->root + "/" + data.h._path.replace(data.h._path.find(l->location), l->location.size(), "");
			struct stat file_status;

			if (stat(full_path.c_str(), &file_status) == -1 || file_status.st_mode & S_IFREG)
				fd = open(full_path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
			else if (file_status.st_mode & S_IFDIR)
				fd = open(string(full_path + "/" + ft::create_date()).c_str(), O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
			if (fd == -1)
				throw Plebception(ERR_FD, "return_post", full_path);
			if (ft::write(fd, data.buf) == -1)
				throw Plebception(ERR_WRITING, "return_post", "");
			close(fd);
			response_code = 201;
		}
	}
	return (data.h.create_header(response_code, data.buf.size()) + string(data.buf));
}

string	Server::return_delete(connect_data &data, Location* l)
{
	int		response_code = 204;
	string  full_path = l->root + "/" + data.h._path.replace(data.h._path.find(l->location), l->location.size(), "");

	if (unlink(full_path.c_str()) == -1)
		response_code = 403;
	return (data.h.create_header(response_code, 0));
}

string	Server::return_put(connect_data &data, Location* l)
{
	string		full_path = l->upload_store + "/" + data.h._path.replace(data.h._path.find(l->location), l->location.size(), "");
	struct stat	file_status;
	int			response_code = 201;
	int			fd;

	if (data.buf.size() > l->max_body_size)
	{
		response_code = 413;
		data.buf.clear();
		err_code_file(data.buf, response_code);
		return (data.h.create_header(response_code, data.buf.size()) + string(data.buf));
	}
	if (stat(full_path.c_str(), &file_status) == -1)
		fd = open(full_path.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0777);
	else if (file_status.st_mode & S_IFDIR)
	{
		response_code = 409;
		data.buf.clear();
		err_code_file(data.buf, response_code);
		return (data.h.create_header(response_code, data.buf.size()) + string(data.buf));
	}
	else
	{
		fd = open(full_path.c_str(), O_TRUNC | O_RDWR, 0777);
		response_code = 204;
	}
	if (fd == -1)
		throw Plebception(ERR_FD, "opening file failed", full_path);
	else
	{
		if (ft::write(fd, data.buf) == -1)
			throw Plebception(ERR_WRITING, "writing to file", full_path);
	}
	close(fd);
	return(data.h.create_header(response_code, 0));
}


string	Server::return_options(connect_data &data, Location* l)
{
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
	data.h.add_to_header_out("Allow", allowed);
	return (data.h.create_header(204, 0));
}

string	Server::return_head(connect_data &data, Location* l)
{
	string 	body;
	int		response_code = 200;

	try
	{
		string file_path = l->find_file(data.h, response_code);
		if (!l->run_cgi(data, body, file_path, *this))
			return ("");
		l->read_file(body, file_path);
	}
	catch(const Plebception& e)
	{
		if (response_code == 404 && ft::ends_with(data.h._path, "/") && l->auto_index == ON)
		{
			response_code = 200;
			create_dirlist(l->root, data.h._path, body);
		}
		else
		{
			std::cerr << e.what() << " response_code: " << response_code << std::endl;
			err_code_file(data.buf, response_code);
		}
	}
	return (data.h.create_header(response_code, body.size()));
}

string	Server::create_response(connect_data &data)
{
	int			response_code = 200;
	Location*	l = match_location(data.h._path);

	if (l == NULL)
		throw Plebception(ERR_NO_LOCATION, "create_response", data.h._path);
	try {l->method_allowed(data.h, response_code); }
	catch (const Plebception& e)
	{
		std::cerr << e.what() << response_code << std::endl;
		return (data.h.create_header(response_code, 0));
	}
	if (l->redir.first != 0)
	{
		data.h.add_to_header_out("Location", l->redir.second);
		return (data.h.create_header(l->redir.first, 0));
	}
	if (data.h._method == "GET")
		return (return_get(data, l));
	if (data.h._method == "POST")
		return (return_post(data, l));
	if (data.h._method == "DELETE")
		return (return_delete(data, l));
	if (data.h._method == "PUT")
		return (return_put(data, l));
	if (data.h._method == "OPTIONS")
		return (return_options(data, l));
	if (data.h._method == "HEAD")
		return (return_head(data, l));
	return (return_post(data, l));
}
