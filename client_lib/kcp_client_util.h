#ifndef _ASIO_KCP_UTIL_
#define _ASIO_KCP_UTIL_

#include <stdint.h>
#include <sys/types.h>

namespace asio_kcp {

void millisecond_sleep(size_t n_millisecond);

/* get system time */
void itimeofday(long *sec, long *usec);

/* get clock in millisecond 64 */
uint64_t iclock64(void);

uint32_t iclock();

} // end of asio_kcp

#endif // _ASIO_KCP_UTIL_
