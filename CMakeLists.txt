cmake_minimum_required(VERSION 3.10)

project(alice_bot)

add_library(alice_bot_lib STATIC src/bot.cpp src/bot.h)

add_executable(alice_bot src/main.cpp)
target_link_libraries(alice_bot alice_bot_lib)

enable_testing()
add_executable(alice_bot_tests tests/main.cpp)
target_link_libraries(alice_bot_tests alice_bot_lib)

add_subdirectory(googletest)
target_link_libraries(alice_bot gtest_main gmock_main)
target_link_libraries(alice_bot_tests gtest_main gmock_main)