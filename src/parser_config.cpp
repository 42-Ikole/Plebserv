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

#include <algorithm>
#include <iostream>

#include <server.hpp>
#include <utilities.hpp>

using namespace std;

static vector<Server>	get_servers(vector<string> lines)
{
	vector<Server>	res;
	int				depth;
	
	for (vector<string>::iterator it = lines.begin(); it != lines.end();)
	{
		depth = 1;
		string s = ft::ltrim(*it);
		if (s == "server {")
		{
			vector<string>::iterator start = it++;
			while (depth != 0)
			{
				s = ft::ltrim(*it);
				if (s.find("{") != string::npos)
					depth++;
				else if (s.find("}") != string::npos)
					depth--;
				it++;
			}
			res.push_back(Server(vector<string>(start, it)));
		}
		else
			it++;
	}
	return (res);
}

static void	depth_check(vector<string> lines, string filename)
{
	int depth = 0;
	for (size_t i = 0; i < lines.size(); i++)
	{
		depth += count(lines[i].begin(), lines[i].end(), '{');
		depth -= count(lines[i].begin(), lines[i].end(), '}');
	}
	if (depth != 0)
		throw Plebception(ERR_BLOCK, filename, "go to dentist, missing brackets: " + ft::to_string(depth));
}

vector<Server> load_config(string filename)
{
	vector<string>	lines = ft::get_lines(filename);
	if (lines.size() < 3)
		throw Plebception(ERR_INVALID_CONFIG, filename, "missing config"); 
	depth_check(lines, filename);
	vector<Server>	res = get_servers(lines);
	return (res);
}
