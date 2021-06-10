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

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <vector>
# include <iostream>
# include <unistd.h>
# include <fcntl.h>
# include <sys/stat.h>

# include <cgi.hpp>
# include <header.hpp> 

class Server;
using namespace std;

enum http_methods
{
	GET,
	HEAD,
	POST,
	PUT,
	DELETE,
	CONNECT,
	OPTIONS,
	TRACE
};

enum auto_index
{
	OFF,
	ON
};

typedef struct	s_lim_except
{
	bool					allow;
	vector<http_methods>	methods;
}				lim_except;

class Location
{
	private:
		vector<http_methods>	_methods;
	public:
		size_t					max_body_size;
		vector<string>			limit_except;
		string					upload_store;
		vector<string>			index_page;
		bool					auto_index;
		string					location;
		pair<int, string>		redir;
		string					root;
		vector<Cgi>				cgi;
		map<string, string>		static_files;
		bool					static_dir;
		Location();
	
	public:
		virtual ~Location();
		Location(vector<string> input);
		Location(const Location& tbc);
		Location& operator=(const Location& tba);

		void 	call(const string& s, vector<string> val);
		int		parse_args(string str);
		void 	set_root(vector<string> val);
		void 	set_auto_index(vector<string> val);
		void 	set_index_page(vector<string> val);
		void 	set_limit_except(vector<string> val);
		void 	set_cgi_pass(vector<string> val);
		void	set_upload_store(vector<string> val);
		void	set_redir(vector<string> val);
		void	load_client_max_body_size(vector<string> val);
		bool	run_cgi(Header& h, string& body, string file_path, Server& ser);
		bool	method_allowed(Header& h, int& response_code);
		void 	set_static_dir(vector<string> val);

		void	read_file(string&  rv, string path);
		char*	read_file(size_t len);
		string	find_file(Header h, int& response_code);
};

std::ostream &operator<<(std::ostream& out, Location const& value);

#endif
