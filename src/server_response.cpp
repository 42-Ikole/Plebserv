#include <server.hpp>

/*
	- de path matchen met een location
	- de file readen
	- response header maken
*/
Location	*Server::match_location(string path)
{
	for (size_t i = 0; i < _locations.size(); i++)
	{
		if (_locations[i]._location.rfind(path, 0) != string::npos)
			return (&_locations[i]);
	}
	//path to 404;
	return (NULL);
}

string	Server::create_response(Header h)
{
	return ("");
	// Location l = match_location(h._path);
	// int response_code = 0;

	// string body = l.find_file(h, &response_code);

	// string response = Header(h).create_header(body, response_code);

	// return (response);
}

// GET / HTTP/1.1