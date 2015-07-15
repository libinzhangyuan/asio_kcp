#include <functional>
#include <future>
#include <chrono>
#include <thread>

#include "../gtest_util.hpp"
#include "../../essential/utility/timer.hpp"


TEST(TimerTest, Normal) {
    Essential::Timer t;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::cout << "nano:" << t.elapsed_nano()
        << "  micro:" << t.elapsed_micro()
        << "  milli:" << t.elapsed()
        << std::endl;
    EXPECT_GE(t.elapsed(), 1);
    EXPECT_GE(t.elapsed_micro(), 1000);
    EXPECT_GE(t.elapsed_nano(), 1000 * 1000);

    t.reset();
    volatile size_t i = 0;
    volatile size_t sum = 0;
    for (i = 0; i < 1000; ++i)
        sum += i;
    std::cout << "nano:" << t.elapsed_nano()
        << "  micro:" << t.elapsed_micro()
        << "  milli:" << t.elapsed()
        << std::endl;
    EXPECT_GE(t.elapsed_nano(), 1);
}
