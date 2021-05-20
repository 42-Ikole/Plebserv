#include <server.hpp>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <iostream>     // std::cout


struct server_data
{
	int fd;
	struct sockaddr_in server_addr;
	Server	*ser;
};

struct	connect_data
{
	int fd;
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
	/*
		hier doen we dingen enzo

	*/
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
	/*
		hier doen we dingen enzo

	*/
	char buffer[1025];
	std::string returnval = "HTTP/1.1 200 OK Date: Thu, 08 Apr 2004 18:24:33 GMT Server: Apache/1.3.29 (Unix) PHP/4.3.4 X-Powered-By: PHP/4.3.4 Content-Language: nl Content-Type: text/html; charset=iso-8859-1 X-Cache: MISS from wikipedia.org Connection: close Content-Type: text/html Content-Length: 49\n\n";
	returnval += "victor je moeter";
	
	std::cout << "We've got an connection! using server " << *client_socket.ser << std::endl;

	int valread = read( client_socket.fd , buffer, 1024);

	// printf("%s\n", buffer);
	send(client_socket.fd , returnval.c_str() , returnval.length() , 0 );
	close(client_socket.fd);

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
	fd_set current_sockets, ready_sockets;

	FD_ZERO(&current_sockets);
	for (size_t i = 0; i < data.size(); i++)
	{
		FD_SET(data[i].fd, &current_sockets);
	}
	while (true)
	{
		ready_sockets = current_sockets;

		if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0)
			perror("sockets");
		for (size_t fd_match = 0; fd_match < FD_SETSIZE; fd_match++)
		{
			if (FD_ISSET(fd_match, &ready_sockets))
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
			else 
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
		}
	}
}
