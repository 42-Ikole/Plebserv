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

#ifndef HEADER_HPP
#define HEADER_HPP

#include <string>
#include <vector>
#include <plebception.hpp>
#include <utilities.hpp>
#include <iomanip>
#include <map>


using namespace std;

class Header
{
public:
	string	_method;
	string	_path;
	string	_query;
	string	_extension;
	string	_http_version;
	bool	_chonky;
	map<string, string> _headers_in;
	map<string, string> _headers_out;
public:
	string _cookies;
	Header();
	Header(vector<std::string> in);
	Header &operator=(const Header &h);
	Header(const Header &h);
	// virtual ~Header();

	void	Parse_request(string request);
	string	content_type_switch();
	string	create_header(int response_code, int body_lengtha);
	void	load_headers_in(vector<string> in);
	void	add_to_header_out(string val, string key);
	void	add_to_header_out(vector<string> head);
	string	decode_url(string &str);
	int		validate_header();

};

std::ostream &operator<<(std::ostream &out, Header const &value);

#endif
