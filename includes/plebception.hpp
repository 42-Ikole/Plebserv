/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*	 																				 */
/*     _ (`-.              ('-. .-. .-')    .-')      ('-.  _  .-')        (`-.   	 */
/*    ( (OO  )           _(  OO)\  ( OO )  ( OO ).  _(  OO)( \( -O )     _(OO  )_ 	 */
/*   _.`     \ ,--.     (,------.;-----.\ (_)---\_)(,------.,------. ,--(_/   ,. \	 */
/*  (__...--'' |  |.-')  |  .---'| .-.  | /    _ |  |  .---'|   /`. '\   \   /(__/	 */
/*   |  /  | | |  | OO ) |  |    | '-' /_)\  :` `.  |  |    |  /  | | \   \ /   / 	 */
/*   |  |_.' | |  |`-' |(|  '--. | .-. `.  '..`''.)(|  '--. |  |_.' |  \   '   /, 	 */
/*   |  .___.'(|  '---.' |  .--' | |  \  |.-._)   \ |  .--' |  .  '.'   \     /__)	 */
/*   |  |      |      |  |  `---.| '--'  /\       / |  `---.|  |\  \     \   /    	 */
/*   `--'      `------'  `------'`------'  `-----'  `------'`--' '--'     `-'     	 */
/*																					 */
/* 									MADE BY											 */
/* 		—————————————————————————————————————————————————————————————————————		 */
/*				 Alpha_1337k       |    https://github.com/Alpha1337k				 */
/*				 VictorTennekes    |    https://github.com/VictorTennekes			 */
/*				 Kingmar	 	   |    https://github.com/K1ngmar					 */
/*																					 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef PLEBCEPTION_HPP
# define PLEBCEPTION_HPP

# include <exception>
# include <string>

# define ERR_SERVER				"Internal server error"
# define ERR_SERVER_FATAL		"Fatal server error"			
# define ERR_SEMICOLON			"WHOA, looks like you forgot a semicolon"
# define ERR_INVALID_TOKEN		"Invalid token found in"
# define ERR_INVALID_VALUE		"Invalid value"
# define ERR_NO_VALUE			"No value found"
# define ERR_BLOCK				"Looks like you forgot to close block"
# define ERR_OUT_OF_RANGE		"Value is out of range"
# define ERR_TOO_MANY_ARG		"You have too many arguments"
# define ERR_TOO_FEW_ARG		"You have too few arguments"
# define ERR_INVALID_AMOUNT_ARG "Invalid amount of arguments"
# define ERR_INVALID_ARG		"Invalid argument"
# define ERR_INVALID_CONFIG 	"Yarr, This config is not valid"
# define ERR_UNKNOWN_IDENT		"OI AMIGO, what is this identifier?"
# define ERR_MULTIPLE_DOM		"Looks like you are defining multiple domains"
# define ERR_FD					"Couldn't open fd"
# define ERR_READ				"Couldn't read fd"
# define ERR_NO_LOCATION		"Can't find matching location"
# define ERR_BAD_LOCATION		"Location not valid"
# define ERR_INVALID_VECTOR		"Vector is invalid"
# define ERR_FAIL_SYSCL			"Failed System call"
# define ERR_NO_CONNECT			"No connection found"
# define ERR_READ_SOCK			"An error occured while reading from socket"
# define ERR_WRITE_SOCK			"An error occured while writing to socket"
# define ERR_BAD_ALLOC			"Something went wrong when allocating memory"

# define COLOR_RED				"\033[31;01m"
# define COLOR_GREEN			"\033[32m"
# define COLOR_RESET			"\033[0m"
# define COLOR_YELLOW			"\033[33m"

using namespace std;

class	Fatal : public exception
{
	private:
		Fatal();
	protected:
		string	_msg;
	public:
		Fatal(string msg, string type, string val);
		virtual ~Fatal() throw();
		Fatal(const Fatal &tbc);
		Fatal & operator=(const Fatal &tba);
		const char *what() const throw();
};

class Plebception : public Fatal
{
	private:
		Plebception();
	public:
		Plebception(string msg, string type, string val);
		virtual ~Plebception() throw();
		Plebception(const Plebception &tbc);
		Plebception & operator=(const Plebception &tba);
};

#endif
