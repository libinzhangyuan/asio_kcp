#include <iostream>

#include "gtest/gtest.h"

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    auto ret = RUN_ALL_TESTS();
    std::cout << "RUN_ALL_TESTS return: " << ret << std::endl;
    return 0;
}
