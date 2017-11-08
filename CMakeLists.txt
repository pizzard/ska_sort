cmake_minimum_required (VERSION 3.5)
project (SKA_SORT)

set (CMAKE_CXX_STANDARD 14)

add_executable (ska_sort_tests ska_sort_tests.cpp)
target_link_libraries(ska_sort_tests gtest gtest_main pthread)

add_executable (ska_sort_benchmarks ska_sort_benchmarks.cpp)
target_link_libraries(ska_sort_benchmarks benchmark pthread)
