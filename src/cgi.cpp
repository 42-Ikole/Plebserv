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

void	Cgi::cgi_response(const Header &h, vector<unsigned char> &body, string file_path)
{
	char *env[18];

	env[0] = (char *)"AUTH_TYPE=BASIC";
	env[1] = (char *)"CONTENT_LENGTH=-1"; // h.content-length
	env[2] = (char *)"CONTENT_TYPE=text/html"; // h.content-type
	env[3] = (char *)"GATEWAY_INTERFACE=CGI / 1.1";
	env[4] = (char *)string("PATH_INFO=" + h._path).c_str();
	env[5] = (char *)string("PATH_TRANSLATED=" + h._path).c_str();
	env[6] = (char *)"QUERY_STRING=";
	env[7] = (char *)"localhost";
	env[8] = (char *)string("REQUEST_METHOD=" + h._method).c_str();
	env[9] = (char *)string("REQUEST_URI=" + h._path).c_str();
	env[10] = (char *)"SERVER_NAME=test";
	env[11] = (char *)"SERVER_PORT=8080";
	env[12] = (char *)"SERVER_PROTOCOL=HTTP/1.1";
	env[13] = (char *)"SERVER_SOFTWARE=Plebserv (linux)";
	env[14] = (char *)"REDIRECT_STATUS=200";
	env[15] = NULL;
	// en nog wat meer idc

	read_response(h, env, body, file_path);
}