#ifndef ERROR_HPP
# define ERROR_HPP

# include <string>

# define COLOR_RED		"\033[31;01m"
# define COLOR_GREEN	"\033[32m"
# define COLOR_RESET	"\033[0m"
# define COLOR_YELLOW	"\033[33m"

enum ErrorCode
{
	PARSER,
	SERVER
};

void error(ErrorCode code, std::string msg);

#endif
