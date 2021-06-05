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
#include <netinet/in.h>
#include <arpa/inet.h>

#define HEADER_END "\r\n\r\n"
#define TIMEOUT		300

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

static void	update_action(connect_data * cur_conn)
{
	struct timeval current_time;

	gettimeofday(&current_time, NULL);
	cur_conn->last_action = current_time.tv_sec;
}

static void	clear_stale_connection(vector<connect_data> &open_connections, fd_set &current_sockets)
{
	struct timeval current_time;

	gettimeofday(&current_time, NULL);
	size_t cur_time = current_time.tv_sec;

	for (size_t i = 0; i < open_connections.size(); i++)
		if (cur_time - open_connections[i].last_action > TIMEOUT)
		{
			close(open_connections[i].fd);
			FD_CLR(open_connections[i].fd, &current_sockets);			
			open_connections.erase(open_connections.begin() + i);
			std::cout << "Succesfull removal of " << i << std::endl;
		}
}

server_data	setup_server(Server &ser, short port, int backlog)
{
	server_data res;
	int opt = 1;

	res.ser = &ser;	
	if ((res.fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		throw Plebception(ERR_SERVER_FATAL, "setup_server", "socket set failed");
	if (setsockopt(res.fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt)))
		throw Plebception(ERR_SERVER_FATAL, "setup_server", "setsockopt failed");
	res.server_addr.sin_family = AF_INET; // ipv4
	res.server_addr.sin_addr.s_addr = INADDR_ANY; // localhost ofzo
	res.server_addr.sin_port = htons(port);
	if (fcntl(res.fd, F_SETFL, O_NONBLOCK) == -1)
		throw Plebception(ERR_SERVER_FATAL, "setup_server", "socket set failed");
	if (bind(res.fd, (struct sockaddr *)&res.server_addr, sizeof(res.server_addr)) < 0)
		throw Plebception(ERR_SERVER_FATAL, "setup_server", "bind failed");
	if (listen(res.fd, backlog) < 0)
		throw Plebception(ERR_SERVER_FATAL, "setup_server", "listen failed");
	return (res);
}

static void accept_connect(fd_set &current_sockets, server_data &data, vector<connect_data> &open_connections)	
{
	connect_data opencon = connect_data();
	do
	{
		opencon.fd = accept(data.fd, (struct sockaddr *)&opencon.client_addr, &opencon.addr_size);
		if (opencon.fd < 0)
		{
			if (errno != EWOULDBLOCK)
				throw Plebception(ERR_SERVER, "accept_connect", "accept failed" + ft::to_string(errno));
			continue;
		}
		if (fcntl(opencon.fd, F_SETFL, O_NONBLOCK) == -1)
			throw Plebception(ERR_SERVER, "accept_connect", "fcntl failed");
		opencon.ser = data.ser;
		update_action(&opencon);
		FD_SET(opencon.fd, &current_sockets);
		std::cout << "New connection " << inet_ntoa(opencon.client_addr.sin_addr) << " on port " << opencon.client_addr.sin_port << std::endl;
		open_connections.push_back(opencon);
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
		// throw Plebception(ERR_READ_SOCK, "recv", ft::to_string(fd));
		std::cout << "error kanker sukkel" << std::endl;
		return "";
	}
	if (rc == 0)
	{
		std::cout << "Connection closed" << endl;
		close_conn = true;
		return "";
	}
	buffer[rc] = 0;
	std::cout << "Bytes recieved " << rc << std::endl;
	ret.resize(rc);
	memcpy(&ret[0], buffer, rc);
	free(buffer);
	return (ret);
}

static void	prepare_chunk_body(connect_data * cur_conn, size_t pos, size_t body_size)
{
	string body = ""; 

	cur_conn->buf = cur_conn->buf.substr(pos + 2, cur_conn->buf.length());
	if (body_size > 0)
	{
		body = cur_conn->buf.substr(0, cur_conn->buf.find_first_of("\r\n"));
		cur_conn->buf = cur_conn->buf.substr(cur_conn->buf.find_first_of("\r\n"), cur_conn->buf.length());
	}
	else
		cur_conn->last = true;
	std::cout << cur_conn->h << std::endl;
	try
	{
		cur_conn->response = cur_conn->ser->create_response(cur_conn->h, body);
		cur_conn->ready = true;
		cout << "chonky boi is ready" << endl;
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
		cur_conn->clear();
	}
}

static void	prepare_chunk(connect_data * cur_conn)
{
	size_t	pos;
	size_t	body_size;

	if (cur_conn->response.empty() == false)
		cur_conn->response.clear();
	pos = cur_conn->buf.find_first_of("\r\n");
	if (pos == string::npos)
		return ;
	body_size = ft::stoi(cur_conn->buf.substr(0, pos), "0123456789ABCDEF");
	if (cur_conn->buf.length() - (pos + 2) < body_size)
		return ;
	if (cur_conn->buf.find_first_of("\r\n", pos + 2) != string::npos)
		prepare_chunk_body(cur_conn, pos, body_size);
}

static void	set_header(connect_data * cur_conn, size_t pos)
{
	cur_conn->header_raw = cur_conn->buf.substr(0, pos);
	cur_conn->buf		 = cur_conn->buf.substr(pos + 4, cur_conn->buf.size() - pos);
	std::cout << "Setting header" << std::endl;
	cur_conn->h = Header(ft::split(cur_conn->header_raw, "\r\n"));
	std::cout << cur_conn->h << "\n\n" << std::endl;
}

static void	normal_response(connect_data * cur_conn)
{
	size_t body_size = atoi(cur_conn->h._headers_in["Content-Length"].c_str());

	if (cur_conn->buf.size() < body_size)
		return ;
	cur_conn->response = cur_conn->ser->create_response(cur_conn->h, cur_conn->buf);
	cur_conn->ready = true;
	std::cout << "Response is ready!" << std::endl;
}

static void	read_request(bool & close_conn, size_t & fd, connect_data * cur_conn)
{
	string ret;

	ret = read_sok(1024, close_conn, fd);
		cur_conn->buf += ret;

	if (close_conn == true || cur_conn->ready == true)
		return ;
	size_t	pos = cur_conn->buf.find(HEADER_END);

	if (pos == string::npos && cur_conn->header_raw.empty())
		return ;
	if (cur_conn->header_raw.empty() == true)
		set_header(cur_conn, pos);
	if (cur_conn->h._method == "GET" || cur_conn->h._chonky == false)
		normal_response(cur_conn);
	else if (cur_conn->h._chonky == true)
		prepare_chunk(cur_conn);
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
	update_action(cur_conn);
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
	ssize_t len = 0;

	if (!cur_conn && cur_conn->ready == false)
		return ;
	// sture die hap
	update_action(cur_conn);
	len = send(fd, &cur_conn->response[cur_conn->bytes_send], cur_conn->response.size() - cur_conn->bytes_send, 0);
	if (len == -1)
		throw Plebception(ERR_WRITE_SOCK, "send_data", "");
	cur_conn->bytes_send += len;
	std::cout << "Bytes send " << cur_conn->bytes_send << " total size: " << cur_conn->response.size() << std::endl;
	if (cur_conn->bytes_send == cur_conn->response.size())
		cur_conn->ready = false;

	if (cur_conn->ready == false && (cur_conn->h._chonky == false || cur_conn->last == true))
		cur_conn->clear();
	else if (cur_conn->ready == false)
		prepare_chunk(cur_conn);
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
	int		rval;
	struct timeval to;

	to.tv_sec = 60;

	std::cout << "Waiting on select.." << endl;
	rval = select(FD_SETSIZE, &read_sok, &write_sok, NULL, &to);
	if (rval < 0)
		throw Plebception(ERR_SERVER_FATAL, "connect_handler", "select failed");
	else for (size_t fd_match = 0; fd_match < FD_SETSIZE; fd_match++)	// fd_setzsize naar current highest veranderen
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
		if (FD_ISSET(fd_match, &write_sok))
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
	{
		clear_stale_connection(open_connections, current_sockets);
		connection_handler(current_sockets, data, open_connections);
	}	
}
