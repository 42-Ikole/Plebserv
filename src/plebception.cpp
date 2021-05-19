#include <plebception.hpp>

Plebception::Plebception(string msg, string type, string val)
{
	_msg = COLOR_GREEN + string("[") + type + string("] ") + COLOR_RED + string("Error: ") + COLOR_RESET + msg + string(" <") + val + string(">"); 
}

Plebception::~Plebception()
{}

Plebception::Plebception(const Plebception &tbc)
{
	*this = tbc;
}

Plebception	&Plebception::operator=(const Plebception &tba)
{
	this->_msg = tba._msg;

	return (*this);
}

const char *Plebception::what() const throw()
{
	return this->_msg.c_str();
}
