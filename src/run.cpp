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

	if (bind(res.fd, (struct sockaddr *)&res.server_addr, sizeof(res.server_addr)) < 0)
		perror("bind failed");
	if (listen(res.fd, backlog) < 0)
		perror("listen failed");
	return (res);
}

int accept_connection(int server_socket, struct sockaddr_in server_addr)
{
	struct sockaddr_in client_addr;
	int addr_size = sizeof(server_addr);
	int client_socket;

	client_socket = accept(server_socket, (struct sockaddr *)&server_addr, (socklen_t*)&addr_size);
	if (client_socket < 0)
		perror("accept failed");
	return (client_socket);
}

void	handle_connection(connect_data client_socket)
{
	char buffer[1025];
	
	std::cout << "We've got an connection! on server " << client_socket.ser->_server << std::endl;

	// read function so larger request dont get cut off
	int valread = read( client_socket.fd , buffer, 1024);
	buffer[valread] = 0;

	vector<string> splitted = ft::split(buffer, "\n");
	try
	{
		Header incoming_header = Header(splitted);
		std::cout << incoming_header << std::endl;
		size_t len = 0;
		char *rv = client_socket.ser->create_response(incoming_header, &len);
		send(client_socket.fd , rv , len , 0 );
		close(client_socket.fd);
		free(rv);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		std::cout << "Moving on...." << std::endl;
		close(client_socket.fd);
	}
}

static void accept_connect(fd_set &current_sockets, vector<server_data> &data, vector<connect_data> &open_connections, size_t &fd_match)	
{
	for (size_t i = 0; i < data.size(); i++)
	{
		if (data[i].fd == fd_match)
		{
			connect_data opencon;
			opencon.fd = accept_connection(data[i].fd, data[i].server_addr);
			opencon.ser = data[i].ser;
			FD_SET(opencon.fd, &current_sockets);	// add to select to watch
			open_connections.push_back(opencon);
		}
	}
			
}

static void	erase_connections(fd_set &current_sockets, vector<server_data> &data, vector<connect_data> &open_connections, size_t &fd_match)
{
	for (size_t i = 0; i < open_connections.size(); i++)
	{
		if (open_connections[i].fd == fd_match)
		{
			handle_connection(open_connections[i]);
			FD_CLR(fd_match, &current_sockets);
			open_connections.erase(open_connections.begin() + i);
		}
	}
}

static void	connection_handler(fd_set &current_sockets, vector<server_data> &data, vector<connect_data> &open_connections)
{
	fd_set ready_sockets = current_sockets;

	// timeout missing
	if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0)
		perror("sockets");
	for (size_t fd_match = 0; fd_match < FD_SETSIZE; fd_match++)
	{
		if (FD_ISSET(fd_match, &ready_sockets))
			accept_connect(current_sockets, data, open_connections, fd_match);
		else
			erase_connections(current_sockets, data, open_connections, fd_match);
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
			data.push_back(setup_server(serv[i], serv[i]._port[x] , 3));
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
