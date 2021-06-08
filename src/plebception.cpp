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

#include <plebception.hpp>

Fatal::Fatal(string msg, string type, string val)
{
	_msg = string("[") + COLOR_GREEN + type + COLOR_RESET+ string("] ") + 
		COLOR_RED + string("Error: ") + COLOR_RESET + msg + 
		string(" <") + COLOR_YELLOW + val + COLOR_RESET + string(">"); 
}

Fatal::~Fatal() throw()
{}

Fatal::Fatal(const Fatal& tbc)
{
	*this = tbc;
}

Fatal	&Fatal::operator=(const Fatal& tba)
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

Plebception::Plebception(const Plebception& tbc) : Fatal(tbc)
{
}

Plebception& Plebception::operator=(const Plebception&tba)
{
	this->_msg = tba._msg;
	return *this;
}

Plebception::~Plebception() throw()
{}
