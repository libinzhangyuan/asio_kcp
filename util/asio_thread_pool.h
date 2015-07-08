#ifndef _ASIO_THREAD_POOL_H__
#define _ASIO_THREAD_POOL_H__

#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>

/*  Usage

// create a thread pool of 4 worker threads
ThreadPool pool(4);
 
// queue a bunch of "work items"
for(int i = 0;i<8;++i)
{
    pool.enqueue([i]
    {
        std::cout << "hello " << i << std::endl;
        boost::this_thread::sleep(
            boost::posix_time::milliseconds(1000)
        );
        std::cout << "world " << i << std::endl;
    });
}
*/

// 缺陷： 无法查看有多少任务积累在队列中. 建议使用muduo中的thread_pool
namespace Net
{

namespace Asio
{

class ThreadPool;
 
// our worker thread objects
class Worker {
public:
    Worker(ThreadPool &s) : pool(s) { }
    void operator()();
private:
    ThreadPool &pool; 
};
 
// the actual thread pool
class ThreadPool {
public:
    ThreadPool(size_t);

    ~ThreadPool();

    template<class F>
        void enqueue(F f);
private:
    // need to keep track of threads so we can join them
    std::vector< std::unique_ptr<boost::thread> > workers;
     
    // the io_service we are wrapping
    boost::asio::io_service service;
    boost::asio::io_service::work working;
    friend class Worker;
};
 
} // namespace Asio

} // namespace Net
#endif // _ASIO_THREAD_POOL_H__
