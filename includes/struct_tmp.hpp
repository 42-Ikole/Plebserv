#ifndef STRUCTTMP_HPP
#define STRUCTTMP_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <mutex>
#include <pthread.h>
#include <unistd.h>

class Server;

struct server_data
{
	int 				fd;
	struct sockaddr_in 	server_addr;
	Server*				ser;
};

struct	cgi_session
{
	string	input;
	string	output;
	size_t	read_i;
	size_t	write_i;
	pid_t	pid;
	int		read_s;
	int		write_s;
	int		fd[2][2];

	cgi_session(int fdin[2], int fdout[2] , string &in): input(in)
	{
		fd[0][0] = fdin[0];
		fd[0][1] = fdin[1];
		fd[1][0] = fdout[0];
		fd[1][1] = fdout[1];
		read_i = 0;
		write_i = 0;
		read_s = 1;
		write_s = 1;
	}
	cgi_session(string &in): input(in)
	{
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
	size_t				chunk_size;
	string				header_raw;
	struct	sockaddr_in	client_addr;
	socklen_t			addr_size;
	size_t				last_action;
	size_t				bytes_send;
	string				response;
	bool				ready;
	std::mutex			lox;

	connect_data() {
		this->fd = 0;
		this->type = 0;
		this->cgi_sesh = 0;
		this->ser = 0;
		this->chunk_size  = 0;
		this->last_action = 0;
		this->bytes_send = 0;
		this->ready = 0;
	}
	~connect_data() {}

	connect_data(const connect_data &other) {*this = other;}

	connect_data &operator=(const connect_data &other)
	{
		this->fd 				= other.fd;
		this->type 				= other.type;
		this->cgi_sesh 			= other.cgi_sesh;
		this->ser 				= other.ser;
		this->h 				= other.h;
		this->buf  				= other.buf;
		this->chunk_unchunked	= other.chunk_unchunked;
		this->chunk_size		= other.chunk_size;
		this->header_raw		= other.header_raw;
		this->client_addr		= other.client_addr;
		this->addr_size			= other.addr_size;
		this->last_action		= other.last_action;
		this->bytes_send		= other.bytes_send;
		this->response			= other.response;
		this->ready				= other.ready;

		return (*this);
	}

	void clear()
	{
		chunk_size = 0;
		bytes_send = 0;
		buf.clear();
		chunk_unchunked.clear();
		response.clear();
		header_raw.clear();
		ready = false;
	}
};


struct	run_data_run
{
	fd_set					current_sockets;
	vector<server_data>		data;
	vector<connect_data>	open_connections;
	int						pipe_fd[2];

};

struct	connection_wrapper {
	run_data_run		&rdr;
	size_t				fd_match;
	int					fd_idx;
	fd_set				read_sok;
	fd_set				write_sok;

	connection_wrapper(run_data_run &rd): rdr(rd)
	{
	}

	connection_wrapper(const connection_wrapper &wrp, run_data_run &rd, size_t _fd_match, int _fd_idx): rdr(rd)
	{
		fd_match = _fd_match;
		fd_idx = _fd_idx;		
		this->read_sok = wrp.read_sok;
		this->write_sok = wrp.write_sok;
	}
};

#endif
