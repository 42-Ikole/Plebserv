#include <server.hpp>
#include <fcntl.h>
#include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
#include <string.h>
#include <map>

/*
	- de path matchen met een location
	- de file readen
	- response header maken
*/

static map<int, string> create_map()
{
	map<int, string> m;
	m[100] = "Doorgaan";
	m[101] = "Protocolwissel";
	m[102] = "Processing";

	m[200] = "OK";
	m[201] = "Aangemaakt";
	m[202] = "Aanvaard";
	m[203] = "Niet-gemachtigde informatie";
	m[204] = "Geen inhoud";
	m[205] = "Inhoud opnieuw instellen";
	m[206] = "Gedeeltelijke inhoud";
	m[207] = "Meerdere statussen";

	m[300] = "Meerkeuze";
	m[301] = "Definitief verplaatst";
	m[302] = "Tijdelijk verplaatst";
	m[303] = "Zie andere";
	m[304] = "Niet gewijzigd";
	m[305] = "Gebruik Proxy";
	m[306] = "(Gereserveerd)";
	m[307] = "Tijdelijke omleiding";
	m[308] = "Definitieve omleiding";

	m[400] = "Foute aanvraag";
	m[401] = "Niet geautoriseerd";
	m[402] = "Betaalde toegang";
	m[403] = "Verboden toegang";
	m[404] = "Niet gevonden";
	m[405] = "Methode niet toegestaan";
	m[406] = "Niet aanvaardbaar";
	m[407] = "Authenticatie op de proxyserver verplicht";
	m[408] = "Aanvraagtijd verstreken";
	m[409] = "Conflict";
	m[410] = "Verdwenen";
	m[411] = "Lengte benodigd";
	m[412] = "Niet voldaan aan vooraf gestelde voorwaarde";
	m[413] = "Aanvraag te groot";
	m[414] = "Aanvraag-URL te lang";
	m[415] = "Media-type niet ondersteund";
	m[416] = "Aangevraagd gedeelte niet opvraagbaar";
	m[417] = "Niet voldaan aan verwachting";
	m[418] = "I'm a teapot";
	m[419] = "Pagina verlopen";
	m[422] = "Aanvraag kan niet verwerkt worden";
	m[423] = "Afgesloten";
	m[424] = "Gefaalde afhankelijkheid";
	m[426] = "Upgrade nodig";
	m[428] = "Voorwaarde nodig";
	m[429] = "Te veel requests";
	m[431] = "Headers van de aanvraag te lang";
	m[450] = "Geblokkeerd door Windows Parental Control";
	m[451] = "Toegang geweigerd om juridische redenen.";
	m[494] = "Request Header Too Large";
	m[495] = "Cert Error";
	m[496] = "No Cert";
	m[497] = "HTTP to HTTPS";
	m[498] = "Token expired/invalid";
	m[499] = "Token required";

	m[500] = "Interne serverfout";
	m[501] = "Niet geïmplementeerd";
	m[502] = "Bad Gateway";
	m[503] = "Dienst niet beschikbaar";
	m[504] = "Gateway Timeout";
	m[505] = "HTTP-versie wordt niet ondersteund";
	m[509] = "Bandbreedte overschreden";
	m[510] = "Niet verlengd";
	m[511] = "Netwerkauthenticatie vereist";
	m[522] = "Connectie duurt te lang";
	m[525] = "TLS-handshake mislukt";
	return m;
}

map<int, string> g_http_errors = create_map();

static void	err_code_file(char *rv, int response_code)
{
	int fd;
	int ret = 1;
	char buf[1025];
	char *file;
	size_t i = 0;

	if ((fd = open("html/error_page/error.html", O_RDONLY)) == -1)
		throw Plebception(ERR_FD, "read_file", "/html/error_page/error.html");
	while (ret)
	{
		ret = read(fd, &buf, 1024);
		if (ret < 0)
			throw Plebception(ERR_READ, "read_file", "/html/error_page/error.html");
		if (ret == 0)
			break ;
		buf[ret] = '\0';
		string tmp = string(buf);
		ret = ret - 8 + g_http_errors[response_code].length() - 14;
		tmp.replace(tmp.find("$error_code"), 11, to_string(response_code));
		tmp.replace(tmp.find("$error_message"), 14, g_http_errors[response_code]);
		for (size_t j = 0; j < 1025 && tmp[j]; j++)
			buf[j] = tmp[j];
		buf[ret] = '\0';
		std::cout << "ret: " << ret << "i " << i << std::endl;
		memcpy(&rv[i], buf, ret);
		i += ret;
	}
	cerr << "wazap" << endl;
}

void	read_file(char *rv, string path)
{
	int fd;
	int ret = 1;
	char buf[1025];
	char *file;
	size_t i = 0;

	if ((fd = open(path.c_str(), O_RDONLY)) == -1)
		throw Plebception(ERR_FD, "read_file", path);
	while (ret)
	{
		ret = read(fd, &buf, 1024);
		if (ret < 0)
			throw Plebception(ERR_READ, "read_file", path);
		buf[ret] = '\0';
		std::cout << "ret: " << ret << "i " << i << std::endl;
		memcpy(&rv[i], buf, ret);
		i += ret;
	}
}

Location	*Server::match_location(string path)
{
	for (size_t i = 0; i < _locations.size(); i++)
	{
		std::cout << "Matching [" << _locations[i]._location << "] with [" << path << "]\n";
		if (!strncmp(_locations[i]._location.c_str(), path.c_str(), _locations[i]._location.length()))
			return (&_locations[i]);
	}
	//path to 404;
	return (NULL);
}

char	*Server::create_response(Header h, size_t *len)
{
	Location *l = match_location(h._path);
	if (l == NULL)
		throw Plebception(ERR_NO_LOCATION, "create_response", h._path);
	int response_code = 200;
	size_t	file_size = 0;
	string file_path;
	try
	{
		file_path = l->find_file(h, &response_code, &file_size);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		file_size = 347 - 8 + g_http_errors[response_code].length() - 14; // niet de filesize hardcoden
		response_code = 404;
	}
	
	string response = h.create_header(response_code, file_size);

	std::cout << "response: " <<  response << "\n\nbody_size: " << file_size << " path: " << file_path << std::endl;

	char *rval = (char *)malloc(sizeof(char) * (file_size + response.length() + 3));
	rval[file_size + response.length() + 2] = 0;
	memcpy(rval, response.c_str(), response.length());
	memcpy(&rval[response.length()], "\r\n", 2);

	if (response_code == 200)
		read_file(&rval[response.length() + 2], file_path);
	else
		err_code_file(&rval[response.length() + 2], response_code);
	*len = file_size + response.length() + 2;
	return (rval);
}
