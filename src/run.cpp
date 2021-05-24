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
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

std::string hello = "HTTP/1.1 200 OK\r\nContent-Type: text/html;\r\ncharset=iso-8859-1\r\nConnection: keep-alive\r\nContent-Type: text/html\r\nContent-Length: 16\r\n\r\nHAHAHAHAHAHHAHAH";


struct server_data
{
	int fd;		// sever fd
	struct sockaddr_in server_addr;
	Server	*ser;
};

struct	connect_data
{
	int fd;		// client fd
	Server *ser;
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
	connect_data *current_connection;
	size_t i = 0;

	for (; i < open_connections.size(); i++)
		if (open_connections[i].fd == fd)
		{
			current_connection = &open_connections[i];
			break;
		}
	if (!current_connection)
	{
		std::cout << "Connection not found! " << endl;
		exit(0);
	}
	do
	{
		bzero(buffer, 1024);
		std::cout << "Waiting for recv.." << std::endl;
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
		std::cout << "Bytes recieved " << rc << std::endl;
		vector<string> splitted = ft::split(buffer, "\n");
		try
		{
			Header incoming_header = Header(splitted);
			std::cout << incoming_header << std::endl;
			vector<unsigned char> rv = current_connection->ser->create_response(incoming_header);
			cout << "TOTAL SIZE " << rv.size() << std::endl;
			send(fd , &rv[0] , rv.size() , 0 );
			cout << "Bytes send!" << std::endl;
			if (rc < 0)
			{
				std::cout << "send failed!" << endl;
				close_conn = true;
				break;
			}
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
		if (rc < 0)
		{
			std::cout << "send failed!" << endl;
			close_conn = true;
			break;
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
			for (size_t i = 0; i < data.size(); i++)
				if (fd_match == data[i].fd)
				{
					std::cout << "Listening socket is available!" << endl;
					accept_connect(current_sockets, data[i], open_connections);
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

	for (size_t i = 0; i < serv.size(); i++)
	{
		for (size_t x = 0; x < serv[i]._port.size(); x++)
		{
			std::cout << "Server " << serv[i]._server << " port: " << serv[i]._port[x] << std::endl;
			data.push_back(setup_server(serv[i], serv[i]._port[x] , 32));
		}
	}
	fd_set current_sockets;

	FD_ZERO(&current_sockets);
	for (size_t i = 0; i < data.size(); i++)
	{
		FD_SET(data[i].fd, &current_sockets);
	}
	while (true)
		connection_handler(current_sockets, data, open_connections);
}
