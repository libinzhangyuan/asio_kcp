#ifndef _ASIO_KCP_LOG_HPP__
#define _ASIO_KCP_LOG_HPP__

#include <g2log.h>

// log level: INFO, DEBUG, WARNING, FATAL


#define AK_LOG LOG          // LOG(FATAL) << "This message is FATAL. After log flush -> Abort()";
#define AK_LOGF LOGF        // LOGF(WARNING, "Printf-style syntax is also %s", "available");
#define AK_LOG_IF LOG_IF    // LOG_IF(FATAL, (2>3)) << "This message is not FATAL";
#define AK_LOGF_IF LOGF_IF

// CHECK AND CHECKF will exit the program. same as FATAL
//#define AK_CHECK CHECK
//#define AK_CHECKF CHECKF    // CHECKF(foo(), "if 'false == foo()' then the %s is broken: FATAL), "contract");


#endif // _ASIO_KCP_LOG_HPP__
