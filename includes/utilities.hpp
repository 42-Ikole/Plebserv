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

#ifndef UTILITIES_HPP
# define UTILITIES_HPP

#define PIPE_BUFFER	16384
#define BUFFER 128

# include <vector>

#define HEXADECIMAL "0123456789abcdef"

namespace ft
{
	std::string&				ltrim(std::string& s, std::string delim = " \t");
	std::vector<std::string>	split(std::string str, std::string delim = " \t\n");
	std::string					trim_char(std::string str, char delim);
	int							stoi(std::string number, const std::string base = "0123456789");
	bool 						ends_with(std::string const& value, std::string const& ending);
	std::vector<std::string> 	get_lines(std::string file);
	char* 						strdup(char* str);
	std::string 				to_string(int val);
	void 						str_set(std::string& res, std::string to_push);
	std::string 				create_date();
	std::string 				convert_header(const std::string& str);

	int							read(int fd, std::string& rv, size_t buf_size, size_t &i);
	int							read(int fd, std::string& rv, size_t buf_size);
	int							write(int fd, std::string& body, size_t& i);
	int							write(int fd, std::string& body);
}

#endif
