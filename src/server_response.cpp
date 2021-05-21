#include <server.hpp>
#include <fcntl.h>
#include <unistd.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/stat.h>
#include <string.h>
/*
	- de path matchen met een location
	- de file readen
	- response header maken
*/

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
	*len = file_size + response.length() + 2;
	return (rval);
}
