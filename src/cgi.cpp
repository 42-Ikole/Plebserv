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
		cerr << "WRITE " << i << endl;
		write_size = i + PIPE_BUFFER >= body.size() ? PIPE_BUFFER + i - body.size() : PIPE_BUFFER;
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
		cerr << "READ " << i << endl;
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
	size_t write_i = 0;
	size_t read_i = 0;
	int status = 0, read_s = 1, write_s = 1;

	close(fdin[0]);
	close(fdout[1]);
	if (body.size() == 0)
		return ("");
	while (read_s != 0)
	{
		cerr << "statuses R: " << read_s << " W: " << write_s << endl; 
		if (write_s != 0)
			write_s = cgi_write(fdin, body, write_i);
		read_s = cgi_read(fdout, rval, read_i);			
	}

	cerr << "Peace out bitch" << endl;
	(void)id;
	(void)status;
	// waitpid(id, &status, 0);
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
	if (fcntl(fdout[0], F_SETFL) < 0)
		throw Plebception(ERR_FAIL_SYSCL, "fcnt3l",  "rip");
	if (fcntl(fdout[1], F_SETFL) < 0)
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

/*
	Maybe
	DOCUMENT_ROOT - This reflects the document root directory of the webserver.
*/

void	Cgi::cgi_response(Header& h, string& body, string file_path, Server& ser, size_t& size)
{
	char* 	env[22];
	char* 	cwd_cstr = getcwd(NULL, 0);
	string 	content_type;
	string 	cwd = string(cwd_cstr);
	
	free(cwd_cstr);

	// cout << "eikels\n\n" << h << endl;
	cerr << "body size = " << body.size() << endl;
	env[0]	= create_env_var("AUTH_TYPE", h._headers_in["Authorization"]); // in header
	if (h._chonky)
		env[1] = create_env_var("CONTENT_LENGTH", ft::to_string(body.size()));
	else
		env[1]	= create_env_var("CONTENT_LENGTH", h._headers_in["Content-Length"]);	// is alleen voor POST // in header
	cerr << "env1 = " << env[1] << endl;
	content_type = (h._method == "GET" ? "text/html" : h._headers_in["Content-Type"]);
	env[2]	= create_env_var("CONTENT_TYPE", content_type);
	env[3]	= create_env_var("GATEWAY_INTERFACE", "CGI/1.1"); 
	env[4]	= create_env_var("PATH_INFO", h._path);
	env[5]	= create_env_var("PATH_TRANSLATED", cwd + '/' + file_path);	// bugged
	env[6]	= create_env_var("QUERY_STRING", h._query);
	env[7]	= create_env_var("REMOTE_ADDR", "127.0.0.1");
	env[8]	= create_env_var("REMOTE_IDENT", "");
	env[9]	= create_env_var("REMOTE_USER", ""); // if auth type == Basic use provided else undefined
	env[10]	= create_env_var("REQUEST_METHOD", h._method);
	env[11]	= create_env_var("REQUEST_URI", h._path);
	env[12]	= create_env_var("SCRIPT_NAME", "http://" + ser.server + h._path); // leading part of path component
	env[13]	= create_env_var("SERVER_NAME", ser.server);
	env[14]	= create_env_var("SERVER_PORT", ft::to_string(ser.port[0]));
	env[15]	= create_env_var("SERVER_PROTOCOL", "HTTP/1.1");
	env[16]	= create_env_var("SERVER_SOFTWARE", "Plebserv (linux)");
	env[17]	= create_env_var("REDIRECT_STATUS", "true");
	env[18]	= create_env_var("SCRIPT_FILENAME",  cwd + '/' + file_path);	// bugged
	env[19] = create_env_var("HTTP_ACCEPT", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
	env[20]	= NULL;

	read_response(env, body, cwd + '/' + file_path);
	size_t pos = body.find(HEADER_END);
	cerr << "kut body gvd: " << body.size() << endl;
	if (pos != string::npos)
	{
		std::cerr << "Found Header!!! end: " << pos << endl;
		h.add_to_header_out(ft::split(body.substr(0, pos), "\r\n"));
		body = body.substr(pos + 4);
		size = body.length();
	}
	for (size_t i = 0; i < 21; i++)
		free(env[i]);
}
