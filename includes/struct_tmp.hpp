#ifndef STRUCTTMP_HPP
#define STRUCTTMP_HPP

#include <sys/socket.h>
#include <netinet/in.h>

class Server;

struct	cgi_session
{
	string	input;
	string	output;
	size_t	read_i;
	size_t	write_i;
	int		read_s;
	int		write_s;
	int		fd[2][2];

	cgi_session(int fdin[2], int fdout[2] , string in)
	{
		fd[0][0] = fdin[0];
		fd[0][1] = fdin[1];
		fd[1][0] = fdout[0];
		fd[1][1] = fdout[1];
		input = in;
		read_i = 0;
		write_i = 0;
		read_s = 1;
		write_s = 1;
	}
	cgi_session(string in)
	{
		input = in;
		read_i = 0;
		write_i = 0;
		read_s = 1;
		write_s = 1;
	}
};

struct	connect_data
{
	int					fd;
	int					type;
	cgi_session			*cgi_sesh;
	Server*				ser;
	Header				h;
	string				buf;
	string				chunk_unchunked;
	string				header_raw;
	struct	sockaddr_in	client_addr;
	socklen_t			addr_size;
	size_t				last_action;
	size_t				bytes_send;
	string				response;
	bool				ready;
	bool				last;
	string				_session_cookies;

	void clear()
	{
		bytes_send = 0;
		buf.clear();
		chunk_unchunked.clear();
		response.clear();
		header_raw.clear();
		ready = false;
		last = false;
	}
};

#endif
