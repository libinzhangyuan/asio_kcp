#include <functional>
#include <future>
#include <chrono>
#include <thread>

#include "../gtest_util.hpp"
#include "../../essential/utility/threadsafe_queue.hpp"

//using namespace Essential;
using namespace std::chrono;

TEST(ThreadSafeQueueTest, GrabAll) {
    Essential::threadsafe_queue<std::string> queue;
    EXPECT_EQ(0, queue.grab_all().size());

    queue.push("111111111111");
    queue.push("22222222222222");
    EXPECT_EQ(2, queue.grab_all().size());
    EXPECT_EQ(0, queue.size());
}

TEST(ThreadSafeQueueTest, wait_and_grab_all) {

    // have things in queue
    {
        Essential::threadsafe_queue<std::string> queue;
        queue.push("111111111111");
        queue.push("22222222222222");
        EXPECT_EQ(2, queue.wait_and_grab_all().size());
        EXPECT_EQ(0, queue.size());
    }

    // have nothing in queue
    {
        Essential::threadsafe_queue<std::string> queue;
        std::function<void()> task( [&]() {
                std::this_thread::sleep_for(milliseconds(10));
                queue.push("111111111111");
            }
        );

        steady_clock::time_point t1 = steady_clock::now();
        //duration<uint64_t, std::ratio<1,1000>> d1 = duration_cast<milliseconds>(t1.time_since_epoch());
        //std::cout << d1.count() << std::endl;

        std::thread(std::move(task)).detach(); // launch on a thread
        const std::queue<std::string>& ret = queue.wait_and_grab_all();

        steady_clock::time_point t2 = steady_clock::now();

        duration<uint64_t, std::ratio<1,1000>> time_span = duration_cast<milliseconds>(t2 - t1);

        EXPECT_GT(time_span.count(), 9);
        EXPECT_TRUE(ret.size() == 1);
        EXPECT_EQ(queue.size(), 0);

        /*
        // future from a packaged_task
        std::packaged_task<int()> task([](){ return 7; }); // wrap the function
        std::future<int> f1 = task.get_future();  // get a future
        std::thread(std::move(task)).detach(); // launch on a thread
        std::cout << "Waiting..." << std::flush;
        f1.wait();
        std::cout << "Done!\nResults are: " << f1.get();
        */
    }
}
