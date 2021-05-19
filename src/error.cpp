#include <error.hpp>
#include <iostream>

static std::string error_msg[] = {
	"parser",
};

void error(ErrorCode code, std::string msg)
{
	std::cerr << COLOR_RED << "Error: " << COLOR_RESET << "<" << error_msg[code] << "> " << msg << std::endl;
	exit(1);
}
