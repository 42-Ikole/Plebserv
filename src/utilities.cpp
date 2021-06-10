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

#include <algorithm>
#include <string>
#include <vector>
#include <cstring>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include <plebception.hpp>
#include <utilities.hpp>

namespace ft
{
    std::string& ltrim(std::string& s, std::string delim)
    {
        s.erase(s.begin(), s.begin() + s.find_first_not_of(delim));
        return s;
    }

    std::vector<std::string> split(std::string str, std::string delim)
    {
        std::vector<std::string>    res;
        size_t                      end;
        size_t                      tmp_end;

        for (size_t begin = 0; begin < str.size(); begin++)
        {
            end = str.find_first_of(delim, begin);
            if (end == std::string::npos)
                end = str.size();
            if (begin != end)
                res.push_back(str.substr(begin, end - begin));
            tmp_end = str.find_first_not_of(delim, end + 1);
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

    int stoi(std::string number, const std::string base)
    {
        int res = 0;
        size_t num;

        if (!number.size())
            return (res);
        int sign = (number[0] == '+') - (number[0] == '-');
        for (size_t i = !!sign; i < number.size(); i++) {
            num = base.find(number[i]);
            if (num == std::string::npos)
                continue ;
            res = res * base.size() + num;
        }
        sign += (!sign);
        return (sign * res);
    }

	bool ends_with(std::string const& value, std::string const& ending)
	{
        
		if (ending.size() > value.size()) return false;
		return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
	}

    std::vector<std::string> get_lines(std::string file)
    {
        int		fd	= open(file.c_str(), O_RDONLY);
		string	unparsed;
		
		if (ft::read(fd, unparsed, 2048) == -1)
			throw Plebception(ERR_READ, "get_lines", file.c_str());
        return (ft::split(unparsed, "\n"));
    }

    char *strdup(char* str)
    {
        char* dup = (char *)malloc(sizeof(char) * strlen(str) + 1);
        if (dup)
            strcpy(dup, str);
        return(dup);
    }

	std::string to_string(int val)
	{
		std::string rval;
		bool        isneg = val < 0 ? true : false;

		if (val == 0)
			return (std::string("0"));
		if (val == -2147483648)
			return (std::string("-2147483648"));
		if (val < 0)
			val *= -1;
		while (val != 0)
		{
			rval.insert(rval.begin(), 1, (val % 10) + 48);
			val /= 10;
		}
		if (isneg)
			rval.insert(0, 1, '-');

		return (rval);
	}

    void str_set(string& res, string to_push)
    {
        res.resize(to_push.size());
        memcpy(&res[0], to_push.c_str(), to_push.size());
    }

    string create_date()
    {
        time_t      rawtime;
        struct tm*  timeinfo;
        char        buf[100];

        time (&rawtime);
        timeinfo = gmtime(&rawtime);
        strftime(buf, 100, "%a, %d %b %G %H:%M:%S GMT", timeinfo);
        return (string(buf));
    }

    static void to_upper(char &ch) {
        ch = toupper(static_cast<unsigned char>(ch));
    }

    string convert_header(const string& str)
    {
        string tmp = str;
        replace(tmp.begin(), tmp.end(), '-', '_');
        for_each(tmp.begin(), tmp.end(), to_upper);
        tmp = "HTTP_" + tmp;
        return tmp;
    }
    
	int		read(int fd, std::string& rv, size_t buf_size, size_t &i)
	{
		int ret = 1;
		char *buff = (char *)malloc(sizeof(char) * (buf_size + 1));

        if (!buff)
            throw Plebception(ERR_FAIL_SYSCL, "read", "error allocating region");
		for (;ret > 0; i += ret)
		{
			ret = ::read(fd, buff, buf_size);
			if (ret < 0)
			{
				free(buff);
				return ret;
			}
			buff[ret] = 0;
		    rv.resize(rv.size() + ret);
			memcpy(&rv[i], buff, ret);
		}
		free(buff);
		return (ret);
	}

	int		read(int fd, std::string& rv, size_t buf_size)
	{
		int ret = 1;
		char *buff = (char *)malloc(sizeof(char) * (buf_size + 1));

        if (!buff)
            throw Plebception(ERR_FAIL_SYSCL, "read", "error allocating region");
		for (size_t i = 0; ret > 0; i += ret)
		{
			ret = ::read(fd, buff, buf_size);
			if (ret < 0)
			{
				free(buff);
				return ret;
			}
			buff[ret] = 0;
		    rv.resize(rv.size() + ret);
			memcpy(&rv[i], buff, ret);
		}
		free(buff);
		return (ret);
	}

    int write(int fd, std::string& body, size_t& i)
    {
        size_t write_size;
        int ret;

        for(ret = 1; ret > 0 && i != body.size(); i += ret)
        {
            write_size = i + PIPE_BUFFER >= body.size() ? body.size() - i : PIPE_BUFFER;
            ret = ::write(fd, &body[i], write_size);
        }
        return (ret);
    }

    int write(int fd, std::string& body)
    {
        size_t write_size;
        int ret = 1;

        for(size_t i = 0; ret > 0 && i != body.size(); i += ret)
        {
            write_size = i + PIPE_BUFFER >= body.size() ? body.size() - i : PIPE_BUFFER;
            ret = ::write(fd, &body[i], write_size);
        }
        return (ret);
    }
}
