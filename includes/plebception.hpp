#ifndef PLEBCEPTION_HPP
# define PLEBCEPTION_HPP

# include <exception>
# include <string>
# include <error.hpp>

# define ERR_SEMICOLON		"WHOA, looks like you forgot a semicolon"
# define ERR_INVALID_TOKEN	"Invalid token found in"
# define ERR_INVALID_VALUE	"Invalid value"
# define ERR_BLOCK			"Looks like you forgot to close block"
# define ERR_TOO_MANY_ARG	"You have too many arguments"
# define ERR_INVALID_CONFIG "Yarr, This config is not valid"
# define ERR_UNKNOWN_IDENT	"OI AMIGO, what is this identifier?"
# define ERR_MULTIPLE_DOM	"Looks like you are defining multiple domains"

using namespace std;

# ifdef LINUX

	class	Plebception : public exception
	{
		private:
			string	_msg;
			Plebception();
		public:
			Plebception(string msg, string type, string val);
			virtual ~Plebception();
			Plebception(const Plebception &tbc);
			Plebception & operator=(const Plebception &tba);
			const char *what() const throw();
	};

# else

	class	Plebception : public exception
	{
		private:
			string	_msg;
			Plebception();
		public:
			Plebception(string msg, string type, string val);
			virtual ~Plebception() _NOEXCEPT;
			Plebception(const Plebception &tbc);
			Plebception & operator=(const Plebception &tba);
			const char *what() const throw();
	};
	
# endif

#endif
