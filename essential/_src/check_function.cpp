#include <exception>
#include "../check_function.h"

namespace Essential
{

    ::std::string make_check_message(const char* const expression, const char* const fileName, int line, const char* const message)
    {
        char moudleName[1024] = ("");
        //::GetModuleFileNameA(NULL, moudleName, sizeof(moudleName) / sizeof(char));


        char showMessage[1024 * 4] = ("");
        snprintf(showMessage, sizeof(showMessage) / sizeof(char),
                "\r\n"
                "ASSERT: ------------------------------------\r\n\r\n"
                "Moudle Name: %s\r\n"
                "File:        %s\r\n"
                "Line:        [%d]\r\n"
                "\r\n"
                "Expression:  %s\r\n"
                "Message:     %s\r\n"
                "--------------------------------------------- ASSERT END\r\n\r\n"
                ,
                moudleName, fileName, line, expression, message);

        return ::std::string(showMessage);
    }

    std::string _assert_check_printf_msg(const char* const expression, const char* const fileName, int line, const char* const message)
    {
        const std::string& msg = make_check_message(expression, fileName, line, message);
        printf("%s", msg.c_str());
        return msg;
    }

    void do_assert(const char* const expression, const char* const fileName, int line, const char* const message)
    {
        const std::string& msg = _assert_check_printf_msg(expression, fileName, line, message);

        // throw exception
        throw AssertException(msg.c_str());
    }

    void _assert_check( bool condition, const char* const expression, const char* const fileName, int line, const char* const message )
    {
        if (!condition)
        {
            do_assert(expression, fileName, line, message);
        }
    }

}
