#ifndef PLEBCEPTION_HPP
# define PLEBCEPTION_HPP

# include <exception>
# include <string>
# include <error.hpp>

using namespace std;

class	Plebception : public exception
{
	private:
		string	_msg;
		Plebception();
	public:
		Plebception(string msg, string type, string val);
		~Plebception();
		Plebception(const Plebception &tbc);
		Plebception & operator=(const Plebception &tba);
		const char *what() const throw();
};

#endif
