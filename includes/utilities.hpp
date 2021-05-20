#ifndef UTILITIES_HPP
# define UTILITIES_HPP


namespace ft
{
	std::string& ltrim(std::string& s, std::string delim = " \t");
	std::vector<std::string> split(std::string str, std::string delim = " \t\n");
	std::string trim_char(std::string str, char delim);
	int stoi(std::string number, const std::string base = "0123456789");
}

#endif