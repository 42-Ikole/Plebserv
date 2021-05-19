#include <location.hpp>


Location::~Location()
{

}

Location::Location()
{

}

Location::Location(const Location& tbc)
{
	*this = tbc;
}

Location& Location::operator=(Location const& tba)
{
	_methods = tba._methods;
	_limit_except = tba._limit_except;
	_root = tba._root;
	_auto_index = tba._auto_index;
	_index_page = tba._index_page;
	return *this;
}
