#include <iostream>
#include <memory>
#include "../googletest/googlemock/include/gmock/gmock.h"
#include "../googletest/googletest/include/gtest/gtest.h"
#include "../src/bot.h"

TEST(TestGroupName, check_init_correct_file) {
    std::unique_ptr<Bot> bot = std::make_unique<Bot>();
    const auto result = bot->init("../inputFiles/in2.txt", 7, 11, 23, 1);
    ASSERT_TRUE(result == true);
}

TEST(TestGroupName, check_init_incorrect_file) {
    std::unique_ptr<Bot> bot = std::make_unique<Bot>();
    const auto result = bot->init("../inputFiles/in1.txt", 7, 11, 23, 1);
    ASSERT_TRUE(result == false);
}

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();

    return 0;
}