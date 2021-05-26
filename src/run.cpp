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
#include <sstream> //tmp
#include <sys/ioctl.h>
#include <sys/time.h>
#include <errno.h>
struct server_data
{
	int fd;		// server fd
	struct sockaddr_in server_addr;
	Server	*ser;
};

struct	connect_data
{
	int fd;		// client fd
	Server *ser;
	vector<unsigned char> buf;
	int i = 0;
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
	connect_data opencon;
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

static void	handle_connection(fd_set &current_sockets, vector<server_data> &data, vector<connect_data> &open_connections, size_t &fd)
{
	bool close_conn = false;
	int rc;
	char buffer[1025];
	bzero(buffer, 1025);
	connect_data *cur_conn;
	size_t i = 0;

	for (; i < open_connections.size(); i++)
	{
		if (open_connections[i].fd == fd)
		{
			cur_conn = &open_connections[i];
			break;
		}
	}
	if (!cur_conn)
	{
		std::cout << "Connection not found! " << endl;
		exit(0); // moet hier wel exit?
	}
	do
	{
		bzero(buffer, 1024);
		rc = recv(fd, buffer, 1024, 0);
		if (rc < 0)
		{
			std::cout << "Recv rval: " << rc <<endl;
			if (errno != EWOULDBLOCK)
			{
				perror("  recv() failed");
				close_conn = true;
			}
			break;
		}
		if (rc == 0)
		{
			std::cout << "Connection closed" << endl;
			close_conn = true;
			break;
		}
		cur_conn->buf.resize(cur_conn->buf.size() + rc);
		memcpy(&cur_conn->buf[cur_conn->i], buffer, rc);
		i += rc;
		std::cout << "Bytes recieved " << rc << std::endl;

		if (rc != 1024)
		{
			try
			{
				const char *crlf2 = "\r\n\r\n";
				auto it = std::search(cur_conn->buf.begin(), cur_conn->buf.end(), crlf2, crlf2 + strlen(crlf2));
				vector<unsigned char> header_part(cur_conn->buf.begin(), it);
				vector<unsigned char> body_part(it, cur_conn->buf.end());
				vector<string> split_header = ft::split(string((char *)&header_part[0]), "\n");
				Header incoming_header = Header(split_header);
				vector<unsigned char> rv = cur_conn->ser->create_response(incoming_header, body_part);
				send(fd, &rv[0], rv.size(), 0);
				cout << "Bytes send!" << std::endl;
				cur_conn->i = 0;
				cur_conn->buf.resize(0);
				if (rc < 0)
				{
					std::cout << "send failed!" << endl;
					close_conn = true;
					break;
				}
			}
			catch(const std::exception& e)
			{
				std::cerr << "Error!" << e.what() << '\n';
				exit(0);
			}
		}
	} while (1);
	if (close_conn)
	{
		close(fd);
		FD_CLR(fd, &current_sockets);
		open_connections.erase(open_connections.begin() + i);
	}
}

static void	connection_handler(fd_set &current_sockets, vector<server_data> &data, vector<connect_data> &open_connections)
{
	fd_set ready_sockets = current_sockets;

	/*
		- todo: select returns the count of fd's to be accepted. we can save cycles by not testing all fd's
	*/
	std::cout << "Waiting on select.." << endl;
	if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) <= 0)
	{
		perror("exiting due to select");
		exit(0);
	}
	for (size_t fd_match = 0; fd_match < FD_SETSIZE; fd_match++)
	{
		if (FD_ISSET(fd_match, &ready_sockets))
		{
			if (fd_match == data[0].fd)
			{
				std::cout << "Listening socket is available!" << endl;
				accept_connect(current_sockets, data[0], open_connections);
			}
			else
			{
				std::cout << "FD " << fd_match << " is readable!" << std::endl;
				handle_connection(current_sockets, data, open_connections, fd_match);
			}
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
