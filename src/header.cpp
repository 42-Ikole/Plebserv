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

#include <iostream>
#include <exception>
#include <map>
#include <ctime>
#include <stdlib.h>

#include <header.hpp>
#include <http_responses.hpp>

using namespace std;

string	Header::decode_url(string& str)
{
	char	rp;

	for (size_t pos = str.find_first_of("%", 0); pos != string::npos; pos = str.find_first_of("%", pos)) {
		rp = ft::stoi(str.substr(pos + 1, 2), HEXADECIMAL);
		str.replace(pos, 3, 1, rp);
	}
	return str;
}

void	Header::Parse_request(string request)
{
	vector<string> parsed = ft::split(request);

	if (parsed.size() != 3)
		throw Plebception(ERR_INVALID_VALUE, "parsing request", "");
	try
	{
		method = parsed[0];
		if (parsed[1].find("?") != string::npos)
		{
			vector<string> tmp = ft::split(parsed[1], "?");
			path = decode_url(tmp[0]);
			query = tmp[1];
		}
		else
			path = decode_url(parsed[1]);
		if (path.find('.') != string::npos)
			extension = path.substr(path.find_last_of('.'), string::npos);
		http_version = parsed[2];
	}
	catch (const Plebception& e)
	{
		std::cerr << e.what() << std::endl;
		throw Plebception(ERR_INVALID_VALUE, "parsing request", "");
	}
}

void	Header::load_headers_in(vector<string> in)
{
	size_t pos;
	for (size_t i = 0; i < in.size(); i++)
	{
		if (in[i].compare(0, 10 ,"Set-Cookie") == 0)
			cookies = cookies + in[i].substr(in[i].find(':') + 2) + ";";
		else
		{
			pos = in[i].find(':');
			headers_in[in[i].substr(0, pos)] = in[i].substr(pos + 2, string::npos);
		}
	}
}

Header::Header() {}

Header&	Header::operator=(const Header& h)
{
	this->method		= h.method;
	this->cookies		= h.cookies;
	this->path			= h.path;
	this->query		= h.query;
	this->extension	= h.extension;
	this->http_version	= h.http_version;
	this->chonky		= h.chonky;
	this->headers_in 	= h.headers_in;
	this->headers_out	= h.headers_out;
	return *this;
}

Header::Header(const Header& h)
{
	*this = h;
}

Header::Header(vector<string> in)
{
	if (in.size() == 0)
		throw Plebception(ERR_INVALID_VECTOR, "empty", "");
	Parse_request(in[0]);
	in.erase(in.begin());
	load_headers_in(in);
	chonky = (headers_in["Transfer-Encoding"] == "chunked") ? true : false; 
}

int	Header::validate_header()
{
	if (http_version != "HTTP/1.1")
		return (505);
	if (path.length() > 2000)
		return (414);
	return (0);
}

string	Header::content_type_switch()
{
	if (extension == ".html")
		return ("text/html; charset=utf-8");
	if (extension == ".css")
		return ("text/css; charset=utf-8");
	if (extension == ".jpg")
		return ("image/jpeg");
	if (extension == ".js")
		return ("text/javascript; charset=utf-8");
	if (extension == ".png")
		return ("image/png");
	if (extension == ".json")
		return ("application/json; charset=utf-8");
	if (extension == ".svg")
		return ("image/svg+xml");
	return ("text/html; charset=utf-8");
}

void	Header::add_to_header_out(string val, string key)
{
	headers_out[val] = key;
}

void	Header::add_to_header_out(vector<string> head)
{
	size_t pos;
	string val;
	string key;

	for (size_t i = 0; i < head.size(); i++)
	{
		pos = head[i].find(":");
		if (pos != string::npos)
		{
			val = head[i].substr(0, pos);
			key = head[i].substr(pos + 2);
			headers_out[val] = key;
		}
		else
			throw Plebception(ERR_TOO_FEW_ARG, "add_to_header", head[i]);
	}
}


string Header::create_header(int response_code, int body_length)
{
	string res;

	if (headers_out["Status"].empty() == false)
		res = "HTTP/1.1 " + headers_out["Status"] + " " + g_http_errors[atoi(headers_out["Status"].c_str())] +"\r\n";
	else
		res = "HTTP/1.1 " + ft::to_string(response_code) + " " + g_http_errors[response_code] +"\r\n";

	add_to_header_out("Date", ft::create_date());
	add_to_header_out("Server", "Plebserv/1.3.29 (Unix)");
	add_to_header_out("Connection", "keep-alive");
	add_to_header_out("Content-Type", content_type_switch());
	if (headers_out["Content-Length"].empty() == true)
		add_to_header_out("Content-Length", ft::to_string(body_length));
	add_to_header_out("Cookie", cookies);

	for (map<string, string>::const_iterator i = headers_out.begin(); i != headers_out.end(); i++)
		res += i->first + ": " + i->second + "\r\n";

	res += "\r\n";
	return (res);
}

std::ostream& operator<<(std::ostream& out, Header const& value)
{
	out << "------------ HEADER --------" << std::endl;
	out << std::setw(15) << "REQUEST LINE | " << "[" << value.method << "] [" << value.path << "]" << std::endl;
	out << std::setw(15) << "QUERY | " << value.query << std::endl;
	out << std::setw(15) << "EXTENSION | " << value.extension << std::endl;
	out << std::setw(15) << "CHONKY | " << value.chonky << std::endl;
	out << std::setw(15) << "OTHER HEADERS:\n";
	for (map<string, string>::const_iterator i = value.headers_in.begin(); i != value.headers_in.end(); i++)
		out << i->first << ": " <<  i->second << std::endl;
	out << "------------ DONE ----------" << std::endl;
	return (out);
}
