cmake_minimum_required(VERSION 3.10)

project(http_client_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(http_client_tests
    unit_tests/curl_http_client_test.cpp
)

target_include_directories(http_client_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../src/http_client/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../src/http_client/src
)

target_link_libraries(http_client_tests
    PRIVATE
        http_client
        GTest::GTest
        GTest::Main
        gmock
)

gtest_discover_tests(http_client_tests)
