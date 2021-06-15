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
#include <utilities.hpp>

#define HEADER_END	"\r\n\r\n"

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

void	Cgi::cgi_child(cgi_session &sesh, char* args[3], char** env)
{
	if (dup2 (sesh.fd[FD_OUT][STDOUT_FILENO], 1) < 0)
		throw Plebception(ERR_FAIL_SYSCL, "dup2", "rip1");
	if (dup2 (sesh.fd[FD_IN][STDIN_FILENO], 0) < 0)
		throw Plebception(ERR_FAIL_SYSCL, "dup2", "rip2");
	close(sesh.fd[0][0]);
	close(sesh.fd[0][1]);
    close(sesh.fd[1][0]);
    close(sesh.fd[1][0]);
	if (execve(_full_path.c_str(), args, env) == -1)
	{
		perror("exeve");
		throw Plebception(ERR_FAIL_SYSCL, "read_response", "execve");
	}
	exit(0);
}

int cgi_write(int &fdin, string &body, size_t &i)
{
	if (body.size() == 0)
	{
		close (fdin);
		return (0);
	}
	int ret = ft::write(fdin, body, i);
	if (ret <= 0)
		return (ret);
	close(fdin);
	return 0;
}

int cgi_read(int &fdout, string &body, size_t &i)
{
	int ret;
	if ((ret = ft::read(fdout, body, PIPE_BUFFER, i)) <= 0)
		return (ret);
	close(fdout);
	return 0;
}


void	Cgi::cgi_parent(cgi_session &sesh)
{
	while (sesh.read_s != 0)
	{
		cerr << "statuses R: " << sesh.read_s << " W: " << sesh.write_s << endl;
		if (sesh.write_s != 0)
			sesh.write_s = cgi_write(sesh.fd[FD_IN][STDOUT_FILENO], sesh.input, sesh.write_i);
		sesh.read_s = cgi_read(sesh.fd[FD_OUT][STDIN_FILENO], sesh.output, sesh.read_i);
	}
}

void Cgi::read_response(connect_data &data, char** env, string file_path)
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
	data.cgi_sesh = new cgi_session(fdin, fdout, data.buf);

	data.cgi_sesh->input = data.buf;
	pid_t id = fork();
	if (id == -1)
		throw Plebception(ERR_FAIL_SYSCL, "cgi_read_response", "fork");
	if (id == 0)
		cgi_child(*data.cgi_sesh, args, env);
	close(data.cgi_sesh->fd[0][0]);
	close(data.cgi_sesh->fd[1][1]);
	cerr << "FD Write: " << data.cgi_sesh->fd[FD_IN][1] << " FD READ: " << data.cgi_sesh->fd[FD_OUT][0] << endl;
	data.buf = data.cgi_sesh->output;
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

void	Cgi::cgi_response(connect_data &data, string& body, string file_path, Server& ser)
{
	map<string, string> env_tmp;

	// cerr << "body size = " << body.size() << endl;
	default_env(data.h, body, file_path, ser, env_tmp);
	for(map<string, string>::iterator it = data.h._headers_in.begin(); it != data.h._headers_in.end(); it++)
		env_tmp[ft::convert_header(it->first)] = it->second;

	char **env = create_env_array(env_tmp);
	read_response(data, env, file_path);
	// cerr << data.buf << endl;
	// body = data.buf;

	// cerr << "cgi body size: " << body.size() << endl;
	
	// size_t pos = body.find(HEADER_END);
	// if (pos != string::npos)
	// {
	// 	data.h.add_to_header_out(ft::split(body.substr(0, pos), "\r\n"));
	// 	body = body.substr(pos + 4);
	// }

	for (size_t i = 0; env[i]; i++)
		free(env[i]);
	free(env);
}
