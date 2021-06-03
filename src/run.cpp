#include <server.hpp>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <iostream>     // std::cout
#include <header.hpp>
#include <utilities.hpp>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <errno.h>
#include <plebception.hpp>

#define HEADER_END "\r\n\r\n"

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
	string	response;
	bool	ready;

	void clear()
	{
		buf.clear();
		response.clear();
		header_raw.clear();
		ready = false;
	}
};

server_data	setup_server(Server &ser, short port, int backlog)
{
	server_data res;
	int opt = 1;

	res.ser = &ser;	
	if ((res.fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		perror("socket failed");
	if (setsockopt(res.fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	res.server_addr.sin_family = AF_INET; // ipv4
	res.server_addr.sin_addr.s_addr = INADDR_ANY; // localhost ofzo
	res.server_addr.sin_port = htons(port);
	if (fcntl(res.fd, F_SETFL, O_NONBLOCK) == -1)
	{
		perror("ioctl");
		exit(0);
	}
	if (bind(res.fd, (struct sockaddr *)&res.server_addr, sizeof(res.server_addr)) < 0)
		perror("bind failed");
	if (listen(res.fd, backlog) < 0)
		perror("listen failed");
	return (res);
}

static void accept_connect(fd_set &current_sockets, server_data &data, vector<connect_data> &open_connections)	
{
	connect_data opencon = connect_data();
	do
	{
		opencon.fd = accept(data.fd, NULL, NULL);
		if (opencon.fd < 0)
		{
			if (errno != EWOULDBLOCK)
			{
				perror("  accept() failed");
				exit(0);
			}
			break;
		}
		printf("  New incoming connection - %d\n", opencon.fd);
		if (fcntl(opencon.fd, F_SETFL, O_NONBLOCK) == -1)
		{
			perror("fcntl");
			exit(0);
		}
		opencon.ser = data.ser;
		FD_SET(opencon.fd, &current_sockets);
		open_connections.push_back(opencon);
		// missing max fd resetter
	} while(opencon.fd != -1);
}

static string read_sok(size_t buff_size, bool & close_conn, size_t & fd)
{
	char	*buffer;
	int		rc;
	string	ret;
	
	buffer = (char *)malloc(sizeof(char) * (buff_size + 1));
	if (buffer == NULL)
		throw Plebception(ERR_BAD_ALLOC, "malloc", "region size " + ft::to_string(buff_size));
	rc = recv(fd, buffer, buff_size, 0);
	if (rc < 0)
	{
		close_conn = true;
		throw Plebception(ERR_READ_SOCK, "recv", ft::to_string(fd));
	}
	if (rc == 0)
	{
		std::cout << "Connection closed" << endl;
		close_conn = true;
		return "";
	}
	buffer[rc] = 0;
	std::cout << "Bytes recieved " << rc << std::endl;
	// cout << buffer << endl;
	ret = string(buffer);			// dit is fout sukkel
	free(buffer);
	return (ret);
}

static void	read_request(bool & close_conn, size_t & fd, connect_data * cur_conn)
{
	string ret;

	ret = read_sok(128, close_conn, fd);
		cur_conn->buf += ret;

	if (close_conn == true)
		return ;
	size_t	pos = cur_conn->buf.find(HEADER_END);

	std::cout << "pos: " << pos << std::endl;
	if (pos == string::npos && cur_conn->header_raw.empty())
		return ;
	if (cur_conn->header_raw.empty() == true)
	{
		cur_conn->header_raw = cur_conn->buf.substr(0, pos);
		cur_conn->buf		 = cur_conn->buf.substr(pos + 4, cur_conn->buf.size() - pos);
		std::cout << "Setting header" << std::endl;
		cur_conn->h = Header(ft::split(cur_conn->header_raw, "\r\n"));
	}
	if (cur_conn->h._method == "GET" || cur_conn->h._chonky == false)
	{
		size_t body_size = atoi(cur_conn->h._headers_in["Content-Length"].c_str());
		cerr << "buf.size() = " << cur_conn->buf.size() <<  " cl = " << body_size << endl;
		if (cur_conn->buf.size() < body_size)
			return ;
		cur_conn->response = cur_conn->ser->create_response(cur_conn->h, cur_conn->buf);
		cur_conn->ready = true;
		std::cout << "Response is ready!" << std::endl;
	}
}

static size_t get_cur_conn_index(size_t fd, vector<connect_data>& data)
{
	size_t x = 0;
	for (; x < data.size() && static_cast<size_t>(data[x].fd) != fd; x++);
	return (x == data.size() ? -1 : x);
}

static connect_data * get_cur_conn(size_t fd, vector<connect_data>& data)
{
	size_t x = 0;
	for (; x < data.size() && static_cast<size_t>(data[x].fd) != fd; x++);
	return (x == data.size() ? NULL : &data[x]);
}

static void	handle_connection(fd_set &current_sockets, vector<connect_data> &open_connections, size_t cur_fd, size_t &fd)
{
	bool close_conn = false;
	connect_data	*cur_conn;

	cur_conn = &open_connections[cur_fd];
	read_request(close_conn, fd, cur_conn);
	if (close_conn == true) {
		close(fd);
		FD_CLR(fd, &current_sockets);
		open_connections.erase(open_connections.begin() + cur_fd);
		return ;
	}
}

static void	send_data(size_t &fd, vector<connect_data> &open_connections)
{
	connect_data *cur_conn = get_cur_conn(fd, open_connections);
	if (!cur_conn && cur_conn->ready == false)
		return ;
	// sture die hap
	send(fd, cur_conn->response.c_str(), cur_conn->response.size(), 0);
	cur_conn->clear();
}

static fd_set	get_response_fd(vector<connect_data> &open_connections)
{
	fd_set ret;

	FD_ZERO(&ret);
	for (size_t i = 0; i < open_connections.size(); i++) {
		if (open_connections[i].ready == true)
			FD_SET(open_connections[i].fd, &ret);
	}
	return ret;
}

static void	connection_handler(fd_set &current_sockets, vector<server_data> &data, vector<connect_data> &open_connections)
{
	fd_set	read_sok  = current_sockets;
	fd_set	write_sok = get_response_fd(open_connections);
	size_t cur_fd;

	std::cout << "Waiting on select.." << endl;
	if (select(FD_SETSIZE, &read_sok, &write_sok, NULL, NULL) <= 0)
	{
		perror("exiting due to select");
		exit(0);
	}
	for (size_t fd_match = 0; fd_match < FD_SETSIZE; fd_match++)	// fd_setzsize naar current highest veranderen
	{
		cur_fd = get_cur_conn_index(fd_match, open_connections);
		if (FD_ISSET(fd_match, &read_sok))
		{
			if (fd_match == static_cast<size_t>(data[0].fd))
			{
				std::cout << "Listening socket is available!" << endl;
				accept_connect(current_sockets, data[0], open_connections);
			}
			else if (cur_fd >= 0)
			{
				std::cout << "FD " << fd_match << " is readable!" << std::endl;
				handle_connection(current_sockets, open_connections, cur_fd, fd_match);
			}
		}
		if (FD_ISSET(fd_match, &write_sok)) // check of header al helemaal is gelezen ofzo
		{
			cout << "socket available for writing" << endl;
			send_data(fd_match, open_connections);
		}
	}
}

void	host_servers(vector<Server> serv)
{
	vector<server_data> data;
	vector<connect_data> open_connections;
	fd_set current_sockets;

	for (size_t i = 0; i < serv.size(); i++)
	{
		for (size_t x = 0; x < serv[i]._port.size(); x++)
		{
			std::cout << "Server " << serv[i]._server << " port: " << serv[i]._port[x] << std::endl;
			data.push_back(setup_server(serv[i], serv[i]._port[x] , 32));
		}
	}
	FD_ZERO(&current_sockets);
	for (size_t i = 0; i < data.size(); i++)
		FD_SET(data[i].fd, &current_sockets);
	while (true)
		connection_handler(current_sockets, data, open_connections);
}
