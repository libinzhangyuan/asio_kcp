#include "asio_thread_pool.h"
namespace Net
{

namespace Asio
{

// all the workers do is execute the io_service
void Worker::operator()() { pool.service.run(); }
 
// the constructor just launches some amount of workers
ThreadPool::ThreadPool(size_t threads) : working(service)
{
    for(size_t i = 0;i<threads;++i)
        workers.push_back(
            std::unique_ptr<boost::thread>(
                new boost::thread(Worker(*this))
            )
        );
}
 
// add new work item to the pool
template<class F>
void ThreadPool::enqueue(F f)
{
    service.post(f);
}
 
// the destructor joins all threads
ThreadPool::~ThreadPool()
{
    service.stop();
    for(size_t i = 0;i<workers.size();++i)
        workers[i]->join();
}

} // namespace Asio

} // namespace Net
