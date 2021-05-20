#include <string>
#include <vector>
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
}
