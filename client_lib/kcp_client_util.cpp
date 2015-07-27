#include <sys/time.h>
#include <time.h>
#include <iostream>
#include <errno.h>
#include <strings.h>

#include "kcp_client_util.h"


namespace asio_kcp {

void millisecond_sleep(size_t n_millisecond)
{
    struct timespec sleepTime;
    struct timespec time_left_to_sleep;
    sleepTime.tv_sec = n_millisecond / 1000;
    sleepTime.tv_nsec = (n_millisecond % 1000) * 1000 * 1000;
    while( (sleepTime.tv_sec + sleepTime.tv_nsec) > 0 )
    {
        time_left_to_sleep.tv_sec = 0;
        time_left_to_sleep.tv_nsec = 0;
        int ret = nanosleep(&sleepTime, &time_left_to_sleep);
        if (ret < 0)
            std::cerr << "nanosleep error with errno: " << errno << " " << strerror(errno) << std::endl;

        sleepTime.tv_sec = time_left_to_sleep.tv_sec;
        sleepTime.tv_nsec = time_left_to_sleep.tv_nsec;
    }
}


/* get system time */
void itimeofday(long *sec, long *usec)
{
	struct timeval time;
	gettimeofday(&time, NULL);
	if (sec) *sec = time.tv_sec;
	if (usec) *usec = time.tv_usec;
}

/* get clock in millisecond 64 */
uint64_t iclock64(void)
{
    long s, u;
    uint64_t value;
    itimeofday(&s, &u);
    value = ((uint64_t)s) * 1000 + (u / 1000);
    return value;
}


uint32_t iclock()
{
    return (uint32_t)(iclock64() & 0xfffffffful);
}

} // end of asio_kcp
