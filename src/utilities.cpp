#include <string>
#include <vector>

using namespace std;

std::string &ltrim(std::string &s)
{
	s.erase(s.begin(), s.begin() + s.find_first_not_of(" \t"));
	return s;
}

// Split string {str} on characters in string {delim} and return vector {res} of tokens
std::vector<std::string> split(std::string str, std::string delim)
{
    std::vector<std::string> res;
    size_t end;

    for (size_t begin = 0; begin < str.size(); begin++)
    {
        end = str.find_first_of(delim, begin);
        if (end == string::npos)
            end = str.size();
        if (begin != end)
            res.push_back(str.substr(begin, end - begin));
        size_t tmp_end = str.find_first_not_of(delim, end + 1);
        if (tmp_end != string::npos)
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