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

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdio.h>

#include <cgi.hpp>
#include <server.hpp>

#define HEADER_END	"\r\n\r\n"
#define PIPE_BUFFER	16384

Cgi::Cgi(string path, string match)
{
	_match = match;
	_full_path = path;
}

Cgi::~Cgi() {}

Cgi::Cgi(const Cgi& tbc)
{
	*this = tbc;
}

Cgi& Cgi::operator=(const Cgi& tba)
{
	_full_path = tba._full_path;
	_match = tba._match;

	return (*this);
}

std::ostream& operator<<(std::ostream& out, Cgi const& value)
{
	out << value._match << ":[" << value._full_path << "] ";
	return (out);
}

void	Cgi::cgi_child(int fdin[2], int fdout[2], char* args[3], char** env)
{
	if (dup2 (fdout[1], 1) < 0)
		throw Plebception(ERR_FAIL_SYSCL, "dup2", "rip1");
	if (dup2 (fdin[0], 0) < 0)
		throw Plebception(ERR_FAIL_SYSCL, "dup2", "rip2");
	close(fdin[0]);
	close(fdin[1]);
    close(fdout[0]);
    close(fdout[1]);
	if (execve(_full_path.c_str(), args, env) == -1)
	{
		perror("exeve");
		throw Plebception(ERR_FAIL_SYSCL, "read_response", "execve");
	}
	exit(0);
}

inline	int cgi_write(int fdin[2], string &body, size_t &i)
{
	size_t write_size;
	for (int ret = 1; ret > 0 && i != body.size();)
	{
		write_size = i + PIPE_BUFFER >= body.size() ? body.size() - i : PIPE_BUFFER;
		// cerr << "WRITE " << i << " sz: " << write_size << endl;
		ret = write(fdin[1], &body[i], write_size);
		if (ret <= 0)
			return ret;
		i += ret;
	}
	close(fdin[1]);
	return 0;
}

inline	int cgi_read(int fdout[2], string &body, size_t &i)
{
	char buff[PIPE_BUFFER + 1];

	for (int ret = 1; ret > 0;)
	{
		// cerr << "READ " << i << endl;
		ret = read(fdout[0], buff, PIPE_BUFFER);
		if (ret <= 0)
			return ret;
		buff[ret] = 0;
		body.resize(body.size() + ret);
		memcpy(&body[i], buff, ret);
		i += ret;
	}
	close(fdout[0]);
	return 0;
}


string	Cgi::cgi_parent(int fdin[2], int fdout[2], pid_t id, string& body)
{
	string rval;
	size_t write_i = 0, read_i = 0;
	int read_s = 1, write_s = 1;

	close(fdin[0]);
	close(fdout[1]);
	if (body.size() == 0)
		return ("");
	while (read_s != 0)
	{
		// cerr << "statuses R: " << read_s << " W: " << write_s << endl; 
		if (write_s != 0)
			write_s = cgi_write(fdin, body, write_i);
		read_s = cgi_read(fdout, rval, read_i);			
	}

	// cerr << "Peace out bitch" << endl;
	(void)id;
	return (rval);
}

void Cgi::read_response(char** env, string& body, string file_path)
{
	char* 	args[3];
	int		fdin[2];
	int		fdout[2];

	args[0] = (char *)_full_path.c_str();
	args[1] = (char *)file_path.c_str();
	args[2] = 0;

	if (pipe(fdin) == -1)
		throw Plebception(ERR_FAIL_SYSCL, "cgi_read_response", "pipe");
	if (pipe(fdout) == -1)
		throw Plebception(ERR_FAIL_SYSCL, "cgi_read_response", "pipe");
	if (fcntl(fdin[0], F_SETFL, O_NONBLOCK) < 0)
		throw Plebception(ERR_FAIL_SYSCL, "fcntl1",  "rip");
	if (fcntl(fdin[1], F_SETFL, O_NONBLOCK) < 0)
		throw Plebception(ERR_FAIL_SYSCL, "fcntl2",  "rip");
	if (fcntl(fdout[0], F_SETFL, O_NONBLOCK) < 0)
		throw Plebception(ERR_FAIL_SYSCL, "fcnt3l",  "rip");
	if (fcntl(fdout[1], F_SETFL, O_NONBLOCK) < 0)
		throw Plebception(ERR_FAIL_SYSCL, "fcntl4",  "rip");

	pid_t id = fork();
	if (id == -1)
		throw Plebception(ERR_FAIL_SYSCL, "cgi_read_response", "fork");
	if (id == 0)
		cgi_child(fdin, fdout, args, env);
	else
		body = cgi_parent(fdin, fdout, id, body);
}

char*	Cgi::create_env_var(string key, string value)
{
	return(ft::strdup((char *)string(key + "=" + value).c_str()));
}

char	**Cgi::create_env_array(map<string, string> &env)
{
	char **rval;
	size_t i = 0;

	rval = (char **)malloc(sizeof(char *) * (env.size() + 1));
	for(map<string, string>::iterator it = env.begin(); it != env.end(); it++)
	{
		rval[i] = create_env_var(it->first, it->second);
		i++;
	}
	rval[i] = 0;
	return (rval);
}

/*
	Maybe
	DOCUMENT_ROOT - This reflects the document root directory of the webserver.
*/

void	Cgi::default_env(Header &h, string &body, string &file_path, Server &ser, map<string, string> &env_tmp)
{
	char* 	cwd_cstr = getcwd(NULL, 0);
	string 	cwd = string(cwd_cstr);

	free(cwd_cstr);
	env_tmp["AUTH_TYPE"] = h._headers_in["Authorization"];
	env_tmp["CONTENT_LENGTH"] = h._chonky == true ? ft::to_string(body.size()) : h._headers_in["Content-Length"];
	env_tmp["CONTENT_TYPE"] = h._method == "GET" ? "text/html" : h._headers_in["Content-Type"];
	env_tmp["GATEWAY_INTERFACE"] = "CGI/1.1";
	env_tmp["PATH_INFO"] = h._path;
	env_tmp["PATH_TRANSLATED"] = cwd + '/' + file_path;
	env_tmp["QUERY_STRING"] = h._query;
	env_tmp["REMOTE_ADDR"] = "127.0.0.1";
	env_tmp["REMOTE_IDENT"] = "";
	env_tmp["REMOTE_USER"] = "";
	env_tmp["REQUEST_METHOD"] =	h._method;
	env_tmp["REQUEST_URI"] = h._path;
	env_tmp["SCRIPT_NAME"] = "http://" + ser.server + h._path;
	env_tmp["SERVER_NAME"] = ser.server;
	env_tmp["SERVER_PORT"] = ft::to_string(ser.port[0]);
	env_tmp["SERVER_PROTOCOL"] = "HTTP/1.1";
	env_tmp["SERVER_SOFTWARE"] = "Plebserv (linux)";
	env_tmp["REDIRECT_STATUS"] = "true";
	env_tmp["SCRIPT_FILENAME"] = cwd + '/' + file_path;
	env_tmp["HTTP_ACCEPT"] = "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8";
}

void	Cgi::cgi_response(Header& h, string& body, string file_path, Server& ser)
{
	map<string, string> env_tmp;

	// cerr << "body size = " << body.size() << endl;
	default_env(h, body, file_path, ser, env_tmp);
	for(map<string, string>::iterator it = h._headers_in.begin(); it != h._headers_in.end(); it++)
		env_tmp[ft::convert_header(it->first)] = it->second;

	char **env = create_env_array(env_tmp);
	read_response(env, body, file_path);
	size_t pos = body.find(HEADER_END);
	// cerr << "kut body gvd: " << body.size() << endl;
	if (pos != string::npos)
	{
		//std::// cerr << "Found Header!!! end: " << pos << endl;
		h.add_to_header_out(ft::split(body.substr(0, pos), "\r\n"));
		body = body.substr(pos + 4);
	}
	for (size_t i = 0; env[i]; i++)
		free(env[i]);
	free(env);
}
