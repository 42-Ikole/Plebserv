#include <string>
#include <vector>
#include <string>
#include <vector>

#include <fcntl.h>
#include <unistd.h>

#include <plebception.hpp>

#define BUFFER 128
namespace ft
{
    std::string& ltrim(std::string& s, std::string delim = " \t")
    {
        s.erase(s.begin(), s.begin() + s.find_first_not_of(delim));
        return s;
    }

    // Split string {str} on characters in string {delim} and return vector {res} of tokens
    std::vector<std::string> split(std::string str, std::string delim = " \t\n")
    {
        std::vector<std::string> res;
        size_t end;

        for (size_t begin = 0; begin < str.size(); begin++)
        {
            end = str.find_first_of(delim, begin);
            if (end == std::string::npos)
                end = str.size();
            if (begin != end)
                res.push_back(str.substr(begin, end - begin));
            size_t tmp_end = str.find_first_not_of(delim, end + 1);
            if (tmp_end != std::string::npos)
                end = tmp_end - 1;
            begin = end;
        }
        return res;
    }

    std::string trim_char(std::string str, char delim)
    {
        std::string res;

        for (size_t i = 0; i < str.size(); i++)
            if (str[i] != delim)
                res += str[i];
        return(res);
    }

    int stoi(std::string number, const std::string base = "0123456789")
    {
        int res = 0;

        if (!number.size())
            return (res);
        int sign = (number[0] == '+') - (number[0] == '-');
        for (size_t i = !!sign; i < number.size(); i++) {
            size_t num = base.find(number[i]);
            if (num == std::string::npos)
                continue;
            res = res * base.size() + num;
        }
        sign += (!sign);
        return (sign * res);
    }

	bool ends_with(std::string const & value, std::string const & ending)
	{
		if (ending.size() > value.size()) return false;
		return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
	}

    std::vector<std::string> get_lines(std::string file)
    {
        int 			            fd	= open(file.c_str(), O_RDONLY);
        char			            incoming[BUFFER + 1];
        std::string			        str_buff = "";
        std::vector<std::string>	res;

        if (fd == -1)
            throw Plebception(ERR_FD, "FD", file);
        for (int ret = 1; ret > 0;)
        {
            ret = read(fd, incoming, BUFFER);
            if (ret < 1)
                break ;
            incoming[ret] = '\0';
            str_buff += incoming;
            for (size_t pos = str_buff.find("\n"); pos != string::npos; pos = str_buff.find("\n"))
            {
                res.push_back(str_buff.substr(0, pos));
                str_buff = str_buff.substr(pos + 1, str_buff.size());
            }
        }
        res.push_back(str_buff.substr(0, str_buff.size()));
        return (res);
    }

    char *strdup(char *str)
    {
        char *dup = (char *)malloc(sizeof(char) * strlen(str) + 1);
        if (dup)
            strcpy(dup, str);
        return(dup);
    }
}
