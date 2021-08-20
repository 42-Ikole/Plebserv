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
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <thread>

#define CURR_SESH cur_conn->cgi_sesh

static void accept_connect(fd_set& current_sockets, server_data& data, vector<connect_data>& open_connections)	
{
	connect_data opencon;
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
		opencon.ready = false;
		opencon.cgi_sesh = 0;
		opencon.h = Header();
		update_action(&opencon);
		FD_SET(opencon.fd, &current_sockets);
		std::cout << "New connection " << inet_ntoa(opencon.client_addr.sin_addr) << " on port " << opencon.client_addr.sin_port << " FD: " << opencon.fd << std::endl;
		open_connections.push_back(opencon);
	} while(opencon.fd != -1);
}

static string read_sok(size_t buff_size, bool& close_conn, size_t& fd)
{
	int		rc;
	string	ret;
	
	ret.resize(buff_size);
	rc = recv(fd, &ret[0], buff_size, 0);
	if (rc <= 0)
	{
		close_conn = true;
		return "";
	}
	ret.resize(rc);
	return (ret);
}

static void	get_chunk_body(connect_data* cur_conn, size_t pos, fd_set &current_sockets)
{
	cur_conn->buf = cur_conn->buf.substr(pos + 2);
	if (cur_conn->chunk_size > 0)
	{
		cur_conn->chunk_unchunked += cur_conn->buf.substr(0, cur_conn->chunk_size);
		cur_conn->buf = cur_conn->buf.substr(cur_conn->chunk_size + 2);
		if (cur_conn->buf.size() != 0)
			unchunk_chunk(cur_conn, current_sockets);
	}
	else
	{
		try
		{
			cur_conn->buf.clear();
			cur_conn->buf.swap(cur_conn->chunk_unchunked);
			cur_conn->response = cur_conn->ser->create_response(*cur_conn);
			if (cur_conn->response == "")
				FD_SET(CURR_SESH->fd[FD_OUT][0], &current_sockets);
			else
				cur_conn->ready = true;
		}
		catch (const Plebception& e)
		{
			std::cerr << e.what() << std::endl;
			cur_conn->clear();
		}
	}
}

void	unchunk_chunk(connect_data* cur_conn, fd_set &current_sockets)
{
	size_t	pos;
	string tmp;

	pos = cur_conn->buf.find("\r\n");
	if (pos == string::npos)
		return ;
	tmp = cur_conn->buf.substr(0, pos);
	cur_conn->chunk_size = ft::stoi(tmp, HEXADECIMAL);
	if (cur_conn->buf.length() - (pos + 2) < cur_conn->chunk_size + 2)
		return ;
	get_chunk_body(cur_conn, pos, current_sockets);
}

static void	set_header(connect_data* cur_conn, size_t pos)
{
	cur_conn->header_raw = cur_conn->buf.substr(0, pos);
	cur_conn->buf		 = cur_conn->buf.substr(pos + 4);
	cur_conn->h = Header(ft::split(cur_conn->header_raw, "\r\n"));
}

static void	normal_response(connect_data* cur_conn, fd_set& current_sockets)
{
	size_t body_size = atoi(cur_conn->h.headers_in["Content-Length"].c_str());

	if (cur_conn->buf.size() < body_size)
		return ;
	cur_conn->response = cur_conn->ser->create_response(*cur_conn);
	if (cur_conn->response == "")
		FD_SET(CURR_SESH->fd[FD_OUT][0], &current_sockets);
	else
		cur_conn->ready = true;
}

static void	create_custom_response(connect_data* cur_conn, string data)
{
	cur_conn->response = data;
	cur_conn->ready = true;
}

static void	read_request(bool& close_conn, size_t& fd, connect_data* cur_conn, fd_set& current_sockets)
{
	string ret;

	ret = read_sok(10240, close_conn, fd);

	cur_conn->buf += ret;
	if (close_conn == true || cur_conn->ready == true)
		return ;
	size_t	pos = cur_conn->buf.find(HEADER_END);
	if (pos == string::npos && cur_conn->header_raw.empty())
		return ;
	if (cur_conn->header_raw.empty() == true)
	{
		set_header(cur_conn, pos);
		int res = cur_conn->h.validate_header();
		if (res != 0)
			return create_custom_response(cur_conn, cur_conn->h.create_header(res, 0));
	}
	if (cur_conn->h.method == "GET" || cur_conn->h.chonky == false)
		normal_response(cur_conn, current_sockets);
	else if (cur_conn->h.chonky == true)
		unchunk_chunk(cur_conn, current_sockets);
}

static void	handle_connection(fd_set& current_sockets, vector<connect_data>& open_connections, size_t fd_idx, size_t& fd)
{
	bool 			close_conn = false;
	connect_data*	cur_conn;

	cur_conn = &open_connections[fd_idx];
	update_action(cur_conn);
	read_request(close_conn, fd, cur_conn, current_sockets);
	if (close_conn == true)
		return clear_connection(open_connections, current_sockets, fd_idx);
}

static void	remove_cgi(connect_data *cur_conn, fd_set &current_sockets)
{
	FD_CLR(CURR_SESH->fd[FD_OUT][STDIN_FILENO], &current_sockets);
	close(CURR_SESH->fd[FD_OUT][STDIN_FILENO]);
	close(CURR_SESH->fd[FD_IN][STDOUT_FILENO]);
	kill(CURR_SESH->pid, SIGTERM);
	waitpid(CURR_SESH->pid, NULL, 0);
	delete CURR_SESH;
	cur_conn->cgi_sesh = 0;
	cur_conn->ready = true;
}

static void	handle_cgi_response(connect_data *cur_conn, bool isread, fd_set &current_sockets)
{
	if (isread)
	{
		CURR_SESH->read_s = cgi_read(CURR_SESH->fd[FD_OUT][STDIN_FILENO], \
									CURR_SESH->output);
		if (CURR_SESH->read_s == 0)
		{
			size_t pos = CURR_SESH->output.find(HEADER_END);
			if (pos != string::npos)
			{
				cur_conn->h.add_to_header_out(ft::split(CURR_SESH->output.substr(0, pos), "\r\n"));
				CURR_SESH->output = CURR_SESH->output.substr(pos + 4);
			}
			cur_conn->response = cur_conn->h.create_header(200, CURR_SESH->output.length()) + string(CURR_SESH->output);
		}
		else if (CURR_SESH->read_s < 0)
			create_custom_response(cur_conn, cur_conn->h.create_header(500, 0));
		if (CURR_SESH->read_s <= 0)
			remove_cgi(cur_conn, current_sockets);
	}
	else
	{
		if (CURR_SESH->write_s > 0)
			CURR_SESH->write_s = cgi_write(CURR_SESH->fd[FD_IN][STDOUT_FILENO], CURR_SESH->input, CURR_SESH->write_i);
		if (CURR_SESH->write_s < 0)
		{
			create_custom_response(cur_conn, cur_conn->h.create_header(500, 0));
			remove_cgi(cur_conn, current_sockets);
		}
	}
}

static void	send_data(size_t &fd, vector<connect_data> &open_connections, fd_set &current_sockets, int &fd_idx)
{
	connect_data* 	cur_conn = &open_connections[fd_idx];
	ssize_t 		len = 0;

	if (!cur_conn)
		return;
	if (CURR_SESH && cur_conn->cgi_sesh->fd[FD_IN][1] == (int)fd)
	{
		handle_cgi_response(cur_conn, false, current_sockets);
		return;
	}
	if (cur_conn->ready == false)
		return ;
	update_action(cur_conn);
	len = send(fd, &cur_conn->response[cur_conn->bytes_send], cur_conn->response.size() - cur_conn->bytes_send, 0);
	if (len == -1)
		throw Plebception(ERR_WRITE_SOCK, "send_data", ft::to_string(errno));
	cur_conn->bytes_send += len;
	if (cur_conn->bytes_send >= cur_conn->response.size())
		cur_conn->clear();
}

static fd_set	get_response_fd(const vector<connect_data> &open_connections)
{
	fd_set ret;

	FD_ZERO(&ret);
	for (size_t i = 0; i < open_connections.size(); i++)
	{
		if (open_connections[i].cgi_sesh != 0 && open_connections[i].cgi_sesh->write_s > 0)
			FD_SET(open_connections[i].cgi_sesh->fd[FD_IN][1], &ret);
		else if (open_connections[i].ready == true)
			FD_SET(open_connections[i].fd, &ret);
	}
	return ret;
}

static void	accept_handle_connection(fd_set& current_sockets, \
	vector<connect_data>& open_connections, size_t& fd_match, int &fd_idx)
{
	try
	{
		if (open_connections[fd_idx].cgi_sesh && open_connections[fd_idx].cgi_sesh->fd[FD_OUT][0] == (int)fd_match)
		{
			handle_cgi_response(&open_connections[fd_idx], true, current_sockets);
			return;
		}
		else
			handle_connection(current_sockets, open_connections, fd_idx, fd_match);
	}
	catch (const Plebception& e)
	{
		std::cerr << e.what() << std::endl;
		create_custom_response(&open_connections[fd_idx], open_connections[fd_idx].h.create_header(500, 0));
	}
}

static void fd_worker (connection_wrapper *wrp)
{
	try
	{
		wrp->rdr.open_connections[wrp->fd_idx].lox.lock();
		if (FD_ISSET(wrp->fd_match, &wrp->read_sok))
			accept_handle_connection(wrp->rdr.current_sockets, wrp->rdr.open_connections, wrp->fd_match, wrp->fd_idx);
		if (FD_ISSET(wrp->fd_match, &wrp->write_sok))
			send_data(wrp->fd_match, wrp->rdr.open_connections, wrp->rdr.current_sockets, wrp->fd_idx);

		if (wrp->rdr.open_connections[wrp->fd_idx].ready == true)
			write(wrp->rdr.pipe_fd[FD_OUT], "!", 1);
		wrp->rdr.open_connections[wrp->fd_idx].lox.unlock();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	delete wrp;
}

static void	connection_handler(run_data_run &rdr) 
{
	connection_wrapper	wrp(rdr);
	wrp.read_sok		= wrp.rdr.current_sockets;
	wrp.write_sok		= get_response_fd(wrp.rdr.open_connections);
	std::thread			worker;
	int					rval;
	char				buff[FD_SETSIZE];
	struct timeval		to;
	int					fd_idx;

	to.tv_sec = 30;
	to.tv_usec = 0;
	rval = select(FD_SETSIZE, &wrp.read_sok, &wrp.write_sok, NULL, &to);
	if (rval < 0)
		throw Fatal(ERR_SERVER_FATAL, "connect_handler", "select failed " + ft::to_string(errno));
	else if (rval == 0)
		return;
	if (read(rdr.pipe_fd[FD_IN], &buff, FD_SETSIZE) >= 0)
	{
		return ;
	}
	for (size_t fd_match = 0; fd_match < FD_SETSIZE; fd_match++)
	{
		fd_idx = get_conn(fd_match, rdr.open_connections);
		if (fd_idx != -1)
		{
			if (FD_ISSET(fd_match, &wrp.write_sok) == false && FD_ISSET(fd_match, &wrp.read_sok) == false)
				continue ;
			worker = std::thread(fd_worker, new connection_wrapper(wrp, rdr, fd_match, fd_idx));
			worker.detach();
		}
		else
		{
			int server_idx = get_port_fd(fd_match, rdr.data);
			if (server_idx >= 0 && fd_match == static_cast<size_t>(rdr.data[server_idx].fd))
				accept_connect(rdr.current_sockets, rdr.data[server_idx], rdr.open_connections);
		}
	}
}

void	host_servers(vector<Server> serv)
{
	run_data_run			rdr;

	rdr.open_connections.reserve(FD_SETSIZE);
	for (size_t i = 0; i < serv.size(); i++)
	{
		for (size_t x = 0; x < serv[i].port.size(); x++)
			rdr.data.push_back(setup_server(serv[i], serv[i].port[x] , 128));
	}
	FD_ZERO(&rdr.current_sockets);
	for (size_t i = 0; i < rdr.data.size(); i++)
		FD_SET(rdr.data[i].fd, &rdr.current_sockets);
	if (pipe(rdr.pipe_fd) == -1)
		throw Plebception(ERR_FAIL_SYSCL, "host_server", "failed setting up pipe");
	if (fcntl(rdr.pipe_fd[0], F_SETFL, O_NONBLOCK) < 0)
		throw Plebception(ERR_FAIL_SYSCL, "fcntl1",  "rip");
	if (fcntl(rdr.pipe_fd[1], F_SETFL, O_NONBLOCK) < 0)
		throw Plebception(ERR_FAIL_SYSCL, "fcntl1",  "rip");
	FD_SET(rdr.pipe_fd[FD_IN], &rdr.current_sockets);
	while (true)
	{
		try
		{
			//clear_stale_connection(open_connections, current_sockets);
			connection_handler(rdr);
		}
		catch(const Plebception& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
}
