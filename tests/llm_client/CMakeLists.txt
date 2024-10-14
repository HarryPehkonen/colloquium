cmake_minimum_required(VERSION 3.10)

project(llm_client_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(llm_client_tests
    message_test.cpp
    parameter_test.cpp
    tool_test.cpp
    openai_translator_test.cpp
)

target_include_directories(llm_client_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../src
)

target_link_libraries(llm_client_tests
    PRIVATE
        GTest::GTest
        GTest::Main
        gmock
)

gtest_discover_tests(llm_client_tests)
