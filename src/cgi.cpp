#include <cgi.hpp>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

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

void Cgi::read_response(const Header &h, char **env, vector<unsigned char> &body, string file_path)
{
	char *args[3];
	int fd[2];
	char buff[1025];

	args[0] = (char *)_full_path.c_str();
	args[1] = (char *)file_path.c_str();
	args[2] = 0;

	if (pipe(fd) == -1)
		throw Plebception(ERR_FAIL_SYSCL, "cgi_read_response", "pipe");

	pid_t id = fork();
	if (id == -1)
		throw Plebception(ERR_FAIL_SYSCL, "cgi_read_response", "fork");
	if (id == 0)
	{
		dup2 (fd[1], 1);
		close(fd[0]);
		close(fd[1]);
		execve(_full_path.c_str(), args, env);
		exit(0);
	}
	else
	{
		int status = 0;
		close(fd[1]);

		for (int ret = 1; ret > 0;)
		{
			ret = read(fd[0], buff, 1024);
			buff[ret] = 0;
			body.resize(body.size() + ret);
			memcpy(&body[0], buff, ret);
		}
		std::cout << "Done with cgi!" << endl;
		waitpid(id, &status, 0);
	}
}

char	*Cgi::create_env_var(string key, string value)
{
	string *res = new string;
	*res = key + "=\"" + value + "\"";
	return ((char *)res->c_str());
}

void	Cgi::cgi_response(const Header &h, vector<unsigned char> &body, string file_path)
{
	char *env[19];


	env[0]	= create_env_var("AUTH_TYPE", "BASIC");
	env[1]	= create_env_var("CONTENT_LENGTH", "-1");
	env[2]	= create_env_var("CONTENT_TYPE", "text/html");
	env[3]	= create_env_var("GATEWAY_INTERFACE", "CGI / 1.1"); 
	env[4]	= create_env_var("PATH_INFO", h._path);
	env[5]	= create_env_var("PATH_TRANSLATED", h._path);
	env[6]	= create_env_var("QUERY_STRING", h._query);
	env[7]	= create_env_var("REMOTE_ADDR", "localhost");
	env[8]	= create_env_var("REMOTE_IDENT", "");
	env[9]	= create_env_var("REMOTE_USER", "");
	env[10]	= create_env_var("REQUEST_METHOD", h._method);
	env[11]	= create_env_var("REQUEST_URI", h._path);
	env[12]	= create_env_var("SCRIPT_NAME", "");
	env[13]	= create_env_var("SERVER_NAME", "localhost");
	env[14]	= create_env_var("SERVER_PORT", "8080");
	env[15]	= create_env_var("SERVER_PROTOCOL", "HTTP/1.1");
	env[16]	= create_env_var("SERVER_SOFTWARE", "Plebserv (linux)");
	env[17]	= create_env_var("REDIRECT_STATUS", "200");
	env[18] = NULL;
	// en nog wat meer idc

	read_response(h, env, body, file_path);
}
