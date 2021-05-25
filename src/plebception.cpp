#include <plebception.hpp>

Plebception::Plebception(string msg, string type, string val)
{
	_msg = string("[") + COLOR_GREEN + type + COLOR_RESET+ string("] ") + 
		COLOR_RED + string("Error: ") + COLOR_RESET + msg + 
		string(" <") + COLOR_YELLOW + val + COLOR_RESET + string(">"); 
}

#ifdef LINUX
	Plebception::~Plebception()
	{}
#else
	Plebception::~Plebception() throw()
	{}
#endif

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
