#ifndef STRUCTTMP_HPP
#define STRUCTTMP_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */

struct	connect_data
{
	int					fd;		// client fd
	int					type;
	int					cgifd[2];
	Server*				ser;
	Header				h;
	string				buf;
	string				chunk_unchunked;
	string				header_raw;
	struct	sockaddr_in	client_addr;
	socklen_t			addr_size;
	size_t				last_action;
	size_t				bytes_send;
	string				response;
	bool				ready;
	bool				last;
	string				_session_cookies;

	void clear()
	{
		bytes_send = 0;
		buf.clear();
		chunk_unchunked.clear();
		response.clear();
		header_raw.clear();
		ready = false;
		last = false;
	}
};

#endif
