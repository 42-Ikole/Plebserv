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
#include <utilities.hpp>
#include <cgi.hpp>
#include <server.hpp>

#define HEADER_END	"\r\n\r\n"

Cgi::Cgi(string path, string m)
{
	match = m;
	full_path = path;
}

Cgi::~Cgi() {}

Cgi::Cgi(const Cgi& tbc)
{
	*this = tbc;
}

Cgi& Cgi::operator=(const Cgi& tba)
{
	full_path = tba.full_path;
	match = tba.match;

	return (*this);
}

std::ostream& operator<<(std::ostream& out, Cgi const& value)
{
	out << value.match << ":[" << value.full_path << "] ";
	return (out);
}

void	Cgi::cgi_child(cgi_session &sesh, char* args[3], char** env)
{
	if (dup2 (sesh.fd[FD_OUT][STDOUT_FILENO], 1) < 0)
		throw Plebception(ERR_FAIL_SYSCL, "dup2", "stdout");
	if (dup2 (sesh.fd[FD_IN][STDIN_FILENO], 0) < 0)
		throw Plebception(ERR_FAIL_SYSCL, "dup2", "stdin");
	close(sesh.fd[0][0]);
	close(sesh.fd[0][1]);
    close(sesh.fd[1][0]);
    close(sesh.fd[1][0]);
	if (execve(full_path.c_str(), args, env) == -1)
	{
		std::cerr << "execve failed!!!!" << std::endl;
		perror("exeve");
	}
	exit(0);
}

int cgi_write(int &fdin, string &body, size_t &i)
{
	if (body.size() == i)
	{
		close (fdin);
		return (0);
	}
	int ret = write(fdin, &body[i], i + PIPE_BUFFER >= body.size() ? body.size() - i : PIPE_BUFFER);
	i += i + PIPE_BUFFER >= body.size() ? body.size() - i : PIPE_BUFFER;
	// std::cout << i << " | " << body.size() << std::endl;
	if (ret <= 0)
		close(fdin);
	return ret;
}

int cgi_read(int &fdout, string &body)
{
	string tmp;

	tmp.resize(PIPE_BUFFER);
	int ret = read(fdout, &tmp[0], PIPE_BUFFER);
	if (ret <= 0)
	{
		close(fdout);
		return (ret);
	}
	tmp.resize(ret);
	body += tmp;
	return (ret);
}

void Cgi::read_response(connect_data &data, char** env, string file_path)
{
	char* 	args[3];
	int		fdin[2];
	int		fdout[2];
	struct	stat buf;

	args[0] = (char *)full_path.c_str();
	args[1] = (char *)file_path.c_str();
	args[2] = 0;

	if (stat(full_path.c_str(), &buf) != 0 || (buf.st_mode & S_IXUSR) == 0)
		throw Plebception(ERR_BAD_LOCATION, "cgi", full_path);
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
	data.cgi_sesh = new cgi_session(fdin, fdout, data.buf);

	pid_t id = fork();
	if (id == -1)
		throw Plebception(ERR_FAIL_SYSCL, "cgi_read_response", "fork");
	if (id == 0)
		cgi_child(*data.cgi_sesh, args, env);
	close(data.cgi_sesh->fd[0][0]);
	close(data.cgi_sesh->fd[1][1]);
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
	if (!rval)
		throw Fatal(ERR_BAD_ALLOC, "malloc", "create_env_array");
	for(map<string, string>::iterator it = env.begin(); it != env.end(); it++)
	{
		rval[i] = create_env_var(it->first, it->second);
		i++;
	}
	rval[i] = 0;
	return (rval);
}

void	Cgi::default_env(Header &h, string &body, string &file_path, Server &ser, map<string, string> &env_tmp)
{
	char* 	cwd_cstr = getcwd(NULL, 0);
	string 	cwd = string(cwd_cstr);

	free(cwd_cstr);
	env_tmp["AUTH_TYPE"]			= h._headers_in["Authorization"];
	env_tmp["CONTENT_LENGTH"]		= h._chonky == true ? ft::to_string(body.size()) : h._headers_in["Content-Length"];
	env_tmp["CONTENT_TYPE"]			= h._method == "GET" ? "text/html" : h._headers_in["Content-Type"];
	env_tmp["GATEWAY_INTERFACE"]	= "CGI/1.1";
	env_tmp["PATH_INFO"] 			= h._path;
	env_tmp["PATH_TRANSLATED"] 		= cwd + '/' + file_path;
	env_tmp["QUERY_STRING"] 		= h._query;
	env_tmp["REMOTE_ADDR"] 			= "127.0.0.1";
	env_tmp["REMOTE_IDENT"] 		= "";
	env_tmp["REMOTE_USER"] 			= "";
	env_tmp["REQUEST_METHOD"] 		= h._method;
	env_tmp["REQUEST_URI"] 			= h._path;
	env_tmp["SCRIPT_NAME"] 			= "http://" + ser.server + h._path;
	env_tmp["SERVER_NAME"] 			= ser.server;
	env_tmp["SERVER_PORT"] 			= ft::to_string(ser.port[0]);
	env_tmp["SERVER_PROTOCOL"] 		= "HTTP/1.1";
	env_tmp["SERVER_SOFTWARE"] 		= "Plebserv (linux)";
	env_tmp["REDIRECT_STATUS"] 		= "true";
	env_tmp["SCRIPT_FILENAME"] 		= cwd + '/' + file_path;
	env_tmp["HTTP_ACCEPT"] 			= "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8";
}

void	Cgi::cgi_response(connect_data &data, string& body, string file_path, Server& ser, int &response_code)
{
	map<string, string> env_tmp;

	default_env(data.h, body, file_path, ser, env_tmp);
	for(map<string, string>::iterator it = data.h._headers_in.begin(); it != data.h._headers_in.end(); it++)
		env_tmp[ft::convert_header(it->first)] = it->second;

	char **env = create_env_array(env_tmp);
	try	{
		read_response(data, env, file_path);
	} catch (Plebception &e) {
		response_code = 500;
		throw e;
	}
	for (size_t i = 0; env[i]; i++)
		free(env[i]);
	free(env);
}
