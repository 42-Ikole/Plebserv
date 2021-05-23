#ifndef PLEBCEPTION_HPP
# define PLEBCEPTION_HPP

# include <exception>
# include <string>

# define ERR_SEMICOLON		"WHOA, looks like you forgot a semicolon"
# define ERR_INVALID_TOKEN	"Invalid token found in"
# define ERR_INVALID_VALUE	"Invalid value"
# define ERR_NO_VALUE		"No value found"
# define ERR_BLOCK			"Looks like you forgot to close block"
# define ERR_OUT_OF_RANGE	"Value is out of range"
# define ERR_TOO_MANY_ARG	"You have too many arguments"
# define ERR_TOO_FEW_ARG	"You have too few arguments"
# define ERR_INVALID_CONFIG "Yarr, This config is not valid"
# define ERR_UNKNOWN_IDENT	"OI AMIGO, what is this identifier?"
# define ERR_MULTIPLE_DOM	"Looks like you are defining multiple domains"
# define ERR_FD				"Couldn't open fd"
# define ERR_READ			"Couldn't read fd"
# define ERR_NO_LOCATION	"Can't find matching location"
# define ERR_INVALID_VECTOR	"Vector is invalid"
# define ERR_FAIL_SYSCL		"Failed System call"

# define COLOR_RED		"\033[31;01m"
# define COLOR_GREEN	"\033[32m"
# define COLOR_RESET	"\033[0m"
# define COLOR_YELLOW	"\033[33m"

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
