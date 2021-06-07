#ifndef RUN_HPP
#define RUN_HPP

#include <server.hpp>
#include <string>
#include <iostream>     // std::cout
#include <header.hpp>
#include <utilities.hpp>

#include <sys/time.h>
#include <errno.h>		// mag er later uit niffo
#include <plebception.hpp>
#include <arpa/inet.h>

#define HEADER_END "\r\n\r\n"
#define TIMEOUT		150

struct server_data
{
	int fd;		// server fd
	struct sockaddr_in server_addr;
	Server	*ser;
};

struct	connect_data
{
	int		fd;		// client fd
	Server	*ser;
	Header	h;
	string	buf;
	string	header_raw;
	struct	sockaddr_in	client_addr;
	socklen_t			addr_size;
	size_t	last_action;
	size_t	bytes_send;
	string	response;
	bool	ready;
	bool	last;

	void clear()
	{
		bytes_send = 0;
		buf.clear();
		response.clear();
		header_raw.clear();
		ready = false;
		last = false;
	}
};

server_data		setup_server(Server &ser, short port, int backlog);
void			update_action(connect_data * cur_conn);
void			clear_connection(vector<connect_data> &open_connections, fd_set &current_sockets, size_t i);
int				get_cur_conn_index(size_t fd, vector<connect_data>& data);
int				get_port_fd(size_t fd, vector<server_data>& data);
connect_data 	*get_cur_conn(size_t fd, vector<connect_data>& data);
void			clear_stale_connection(vector<connect_data> &open_connections, fd_set &current_sockets);

#endif
