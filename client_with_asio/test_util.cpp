#include "test_util.h"
#include <sstream>

std::string test_str(const std::string& str_prefix, const size_t str_len)
{
    std::ostringstream ostr;
    ostr << str_prefix << ':';
    std::string str = ostr.str();

    // when len <= prefixLen
    //   test_str(adfadf, 3) == adf
    if (str.size() >= str_len)
    {
        str.resize(str_len);
        return str;
    }

    // when len < 20
    //   test_str(adf, 13) == adfa:67890123
    if (str_len < 20)
    {
        for (size_t i = str.size() + 1; i <= str_len; ++i)
            str.push_back(char(((i % 10) + 0x30)));
        return str;
    }

    // when len >= 20
    //   test_str(asdf, 45) = asdf:=====(20_56789)(30_56789)12345
    {
        // asdf:=====
        str.resize(10, '=');

        // (20_56789)(30_56789)
        while (str.size() + 10 <= str_len)
        {
            const size_t new_len = str.size() + 10;
            std::ostringstream tmp_ostrm;
            tmp_ostrm << str << "(" << new_len << '_';
            str = tmp_ostrm.str();
            for (size_t i = str.size() + 1; i < new_len; ++i)
                str.push_back(char(((i % 10) + 0x30)));
            str += ')';
        }

        // 12345
        if (str.size() < str_len)
        {
            for (size_t i = str.size() + 1; i <= str_len; ++i)
            {
                char c = ((i % 10) + 0x30);
                str.push_back(c);
            }
        }
        return str;
    }

    return str;
}
