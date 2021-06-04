#include <plebception.hpp>

Fatal::Fatal(string msg, string type, string val)
{
	_msg = string("[") + COLOR_GREEN + type + COLOR_RESET+ string("] ") + 
		COLOR_RED + string("Error: ") + COLOR_RESET + msg + 
		string(" <") + COLOR_YELLOW + val + COLOR_RESET + string(">"); 
}

#ifdef LINUX
	Fatal::~Fatal() throw()
	{}
#else
	Fatal::~Fatal() throw()
	{}
#endif

Fatal::Fatal(const Fatal &tbc)
{
	*this = tbc;
}

Fatal	&Fatal::operator=(const Fatal &tba)
{
	this->_msg = tba._msg;
	return (*this);
}

const char *Fatal::what() const throw()
{
	return this->_msg.c_str();
}

Plebception::Plebception(string msg, string type, string val) : Fatal(msg, type, val)
{
}

Plebception::Plebception(const Plebception &tbc)
{
	*this = tbc;
}

Plebception & Plebception::operator=(const Plebception &tba)
{
	*this = tba;
}

