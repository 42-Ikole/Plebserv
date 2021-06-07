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

#include <cgi.hpp>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <server.hpp>
#include <stdio.h>

#define HEADER_END	"\r\n\r\n"
#define GET_REKT	"Solve deze merge dan Oscar"

Cgi::Cgi(string path, string match)
{
	_match = match;
	_full_path = path;
}

Cgi::~Cgi() {}

Cgi::Cgi(const Cgi &tbc)
{
	*this = tbc;
}

Cgi &Cgi::operator=(const Cgi &tba)
{
	_full_path = tba._full_path;
	_match = tba._match;

	return (*this);
}

std::ostream &operator<<(std::ostream &out, Cgi const &value)
{
	out << value._match << ":[" << value._full_path << "] ";
	return (out);
}

void	Cgi::cgi_child(int fdin[2], int fdout[2], char *args[3], char **env)
{
	dup2 (fdout[1], 1);
	dup2 (fdin[0], 0);
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

void	Cgi::cgi_parent(int fdin[2], int fdout[2], pid_t id, string &body)
{
	char buff[1025];

	int status = 0;
	int	i = 0;
	close(fdin[0]);
	close(fdout[1]);

	std::cout << body << endl;
	write(fdin[1], body.c_str(), body.size());
	close(fdin[1]);
	body.resize(0);
	for (int ret = 1; ret > 0;)
	{
		ret = read(fdout[0], buff, 1024);
		buff[ret] = 0;
		body.resize(body.size() + ret);
		memcpy(&body[i], buff, ret);
		i += ret;
	}
	close(fdout[0]);
	std::cout << "Done with cgi!" << endl;
	waitpid(id, &status, 0);
}

void Cgi::read_response(char** env, string &body, string file_path)
{
	char *args[3];
	int fdin[2];
	int	fdout[2];

	args[0] = (char *)_full_path.c_str();
	args[1] = (char *)file_path.c_str();
	args[2] = 0;

	if (pipe(fdin) == -1)
		throw Plebception(ERR_FAIL_SYSCL, "cgi_read_response", "pipe");
	if (pipe(fdout) == -1)
		throw Plebception(ERR_FAIL_SYSCL, "cgi_read_response", "pipe");

	pid_t id = fork();
	if (id == -1)
		throw Plebception(ERR_FAIL_SYSCL, "cgi_read_response", "fork");
	if (id == 0)
		cgi_child(fdin, fdout, args, env);
	else
		cgi_parent(fdin, fdout, id, body);
}

char	*Cgi::create_env_var(string key, string value)
{
	return(ft::strdup((char *)string(key + "=" + value).c_str()));
}

/*
	Maybe
	DOCUMENT_ROOT - This reflects the document root directory of the webserver.
*/

void	Cgi::cgi_response(Header &h, string &body, string file_path, Server &ser, size_t &size)
{
	char *env[22];
	char *cwd_cstr = getcwd(NULL, 0);
	string cwd = string(cwd_cstr);
	free(cwd_cstr);

	// (void)ser;
	// std::cout << "STARTING THE CGI\n\n\n" << h << endl;
	// env[0]	= create_env_var("GATEWAY_INTERFACE", "CGI/1.1"); 
	// env[1]	= create_env_var("SCRIPT_FILENAME", cwd + '/' + file_path);
	// env[2]	= create_env_var("REQUEST_METHOD", h._method);
	// env[3]	= create_env_var("REDIRECT_STATUS", "true");
	// env[4]	= create_env_var("SERVER_PROTOCOL", "HTTP/1.1");
	// env[5]	= create_env_var("REMOTE_HOST", "127.0.0.1");
	// if (h._method == "GET")
	// 	env[6]	= create_env_var("CONTENT_TYPE", "text/html");
	// else
	// 	env[6]	= create_env_var("CONTENT_TYPE", "application/x-www-form-urlencoded");
	// env[7] = create_env_var("HTTP_ACCEPT", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
	// env[8]	= create_env_var("CONTENT_LENGTH", h._headers_in["Content-Length"]);
	// env[9]	= 0;

	env[0]	= create_env_var("AUTH_TYPE", h._headers_in["Authorization"]); // in header
	env[1]	= create_env_var("CONTENT_LENGTH", h._headers_in["Content-Length"]);	// is alleen voor POST // in header
	if (h._method == "GET")
		env[2]	= create_env_var("CONTENT_TYPE", "text/html");
	else
		env[2]	= create_env_var("CONTENT_TYPE", "application/x-www-form-urlencoded");
	env[3]	= create_env_var("GATEWAY_INTERFACE", "CGI/1.1"); 
	env[4]	= create_env_var("PATH_INFO", h._path);
	env[5]	= create_env_var("PATH_TRANSLATED", cwd + '/' + file_path);	// bugged
	env[6]	= create_env_var("QUERY_STRING", h._query);
	env[7]	= create_env_var("REMOTE_ADDR", "127.0.0.1");
	env[8]	= create_env_var("REMOTE_IDENT", "");
	env[9]	= create_env_var("REMOTE_USER", ""); // if auth type == Basic use provided else undefined
	env[10]	= create_env_var("REQUEST_METHOD", h._method);
	env[11]	= create_env_var("REQUEST_URI", h._path);
	env[12]	= create_env_var("SCRIPT_NAME", "http://" + ser._server + h._path); // leading part of path component
	env[13]	= create_env_var("SERVER_NAME", ser._server);
	env[14]	= create_env_var("SERVER_PORT", ft::to_string(ser._port[0]));
	env[15]	= create_env_var("SERVER_PROTOCOL", "HTTP/1.1");
	env[16]	= create_env_var("SERVER_SOFTWARE", "Plebserv (linux)");
	env[17]	= create_env_var("REDIRECT_STATUS", "true");
	env[18]	= create_env_var("SCRIPT_FILENAME",  cwd + '/' + file_path);	// bugged
	env[19] = create_env_var("HTTP_ACCEPT", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
	env[20]	= NULL;
	read_response(env, body, cwd + '/' + file_path);
	

	size_t pos = body.find(HEADER_END);

	if (pos != string::npos)
	{
		std::cout << "Found Header!!! end: " << pos << endl;
		h.add_to_header_out(ft::split(body.substr(0, pos), "\r\n"));
		body = body.substr(pos + 4);
		size = body.length();
	}
	for (size_t i = 0; i < 22; i++)
	{
		free(env[i]);
	}
	
}
