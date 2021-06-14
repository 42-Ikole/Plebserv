/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*	 																				 */
/*     _ (`-.              ('-. .-. .-')    .-')      ('-.  _  .-')        (`-.   	 */
/*    ( (OO  )           _(  OO)\  ( OO )  ( OO ).  _(  OO)( \( -O )     _(OO  )_ 	 */
/*   _.`     \ ,--.     (,------.;-----.\ (_)---\_)(,------.,------. ,--(_/   ,. \	 */
/*  (__...--'' |  |.-')  |  .---'| .-.  | /    _ |  |  .---'|   /`. '\   \   /(__/	 */
/*   |  /  | | |  | OO ) |  |    | '-' /_)\  :` `.  |  |    |  /  | | \   \ /   / 	 */
/*   |  |_.' | |  |`-' |(|  '--. | .-. `.  '..`''.)(|  '--. |  |_.' |  \   '   /, 	 */
/*   |  .___.'(|  '---.' |  .--' | |  \  |.-._)   \ |  .--' |  .  '.'   \     /__)	 */
/*   |  |      |      |  |  `---.| '--'  /\       / |  `---.|  |\  \     \   /    	 */
/*   `--'      `------'  `------'`------'  `-----'  `------'`--' '--'     `-'     	 */
/*																					 */
/* 									MADE BY											 */
/* 		—————————————————————————————————————————————————————————————————————		 */
/*				 Alpha_1337k       |    https://github.com/Alpha1337k				 */
/*				 VictorTennekes    |    https://github.com/VictorTennekes			 */
/*				 Kingmar	 	   |    https://github.com/K1ngmar					 */
/*																					 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <run.hpp>

server_data	setup_server(Server& ser, short port, int backlog)
{
	server_data res;
	int 		opt = 1;

	res.ser = &ser;	
	if ((res.fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		throw Plebception(ERR_SERVER_FATAL, "setup_server", "socket set failed");
	if (setsockopt(res.fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt)))
		throw Plebception(ERR_SERVER_FATAL, "setup_server", "setsockopt failed");
	res.server_addr.sin_family = AF_INET; // ipv4
	res.server_addr.sin_addr.s_addr = INADDR_ANY; // localhost
	res.server_addr.sin_port = htons(port);
	if (fcntl(res.fd, F_SETFL, O_NONBLOCK) == -1)
		throw Plebception(ERR_SERVER_FATAL, "setup_server", "socket set failed");
	if (bind(res.fd, (struct sockaddr *)&res.server_addr, sizeof(res.server_addr)) < 0)
		throw Plebception(ERR_SERVER_FATAL, "setup_server", "bind failed");
	if (listen(res.fd, backlog) < 0)
		throw Plebception(ERR_SERVER_FATAL, "setup_server", "listen failed");
	return (res);
}

void	update_action(connect_data* cur_conn)
{
	struct timeval current_time;

	if (gettimeofday(&current_time, NULL) >= 0)
		cur_conn->last_action = current_time.tv_sec;
	else
		cur_conn->last_action = 420;
}

void clear_connection(vector<connect_data>& open_connections, fd_set& current_sockets, size_t i)
{
	int fd = open_connections[i].fd;

	close(open_connections[i].fd);
	FD_CLR(open_connections[i].fd, &current_sockets);		
	open_connections.erase(open_connections.begin() + i);
	(void)fd;
	// std::cout << "Succesfull removal of " << fd << std::endl;
}

void	clear_stale_connection(vector<connect_data>& open_connections, fd_set& current_sockets)
{
	struct timeval current_time;

	gettimeofday(&current_time, NULL);
	for (size_t i = 0; i < open_connections.size(); i++)
	{
		if (current_time.tv_sec - open_connections[i].last_action > TIMEOUT)
			clear_connection(open_connections, current_sockets, i);
	}
	// cout << open_connections.size() << endl;
}

int get_cur_conn_index(size_t fd, vector<connect_data>& data)
{
	long x = 0;
	for (; x < (long)data.size() && static_cast<size_t>(data[x].fd) != fd && \
	(data[x].cgi_sesh && data[x].cgi_sesh->fd[FD_OUT][0] != (int)fd); x++);
	return (x == (long)data.size() ? -1 : x);
}

int get_port_fd(size_t fd, vector<server_data>& data)
{
	long x = 0;
	for (; x < (long)data.size() && static_cast<size_t>(data[x].fd) != fd; x++);
	return (x == (long)data.size() ? -1 : x);
}

connect_data * get_cur_conn(size_t fd, vector<connect_data>& data)
{
	size_t x = 0;
	for (; x < data.size() && static_cast<size_t>(data[x].fd) != fd && \
	(data[x].cgi_sesh && data[x].cgi_sesh->fd[FD_IN][1] != (int)fd); x++);
	return (x == data.size() ? NULL : &data[x]);
}
