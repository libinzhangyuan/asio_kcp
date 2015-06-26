#ifndef _CHECK_FUNCTION_H_
#define _CHECK_FUNCTION_H_
#include <exception>
#include <string>
#include <string.h>
#include <stdio.h>
#include "es_config.h"

BEGIN_ES_NAMESPACE

#ifdef _UTEST_
    #define assert_check(check, expression) exception_assert(bool(check), (const char*)(expression))
#else
    #define assert_check(condition, message) Essential::_assert_check((condition), (#condition), (__FILE__), __LINE__, (message))
#endif

class AssertException
{
public:
    AssertException(const char *const& _What)
    {
        strncpy(m_What, _What, sizeof(m_What));
        m_What[1023] = 0;
    }
    ~AssertException() {}
private:
    char m_What[1024];
};

#define exception_assert(condition, message) Essential::_excption_check<Essential::AssertException>((condition), (#condition), (__FILE__), __LINE__, (message))

#define do_check(checkFunction, condition, message) checkFunction((condition), (#condition), (__FILE__), __LINE__, (message))



// *****************************************************************
//	下面是实现:
// *****************************************************************
//
	typedef void (*check_fuction)(  bool				/* condition */, 
									const char* const	/* expression */,
									const char* const	/* fileName */,
									int					/* line */,
									const char* const	/* message */);



	void _assert_check( bool condition, const char* const expression, const char* const fileName, int line, const char* const message );


    std::string _assert_check_printf_msg(const char* const expression, const char* const fileName, int line, const char* const message);

	template <class exception_type>
	void _excption_check(bool condition, const char* const expression, const char* const fileName, int line, const char* const message) {
		if (!condition)
		{
            const std::string& msg = _assert_check_printf_msg(expression, fileName, line, message);
			throw exception_type(msg.c_str());
		}
	}





END_ES_NAMESPACE


#endif
