cmake_minimum_required(VERSION 3.10)

# Find GTest package
find_package(GTest REQUIRED)
include(GoogleTest)

# Add http_client tests
add_executable(http_client_tests
    http_client/unit_tests/curl_http_client_test.cpp
)

target_include_directories(http_client_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_directories(http_client_tests PRIVATE ${CMAKE_BINARY_DIR}/lib)

target_link_libraries(http_client_tests
    PRIVATE
        http_client
        GTest::GTest
        GTest::Main
        gmock
)

gtest_discover_tests(http_client_tests)

# Add llm_client tests
add_executable(llm_client_tests
    core/message_test.cpp
    core/parameter_test.cpp
    core/tool_test.cpp
    translator/openai_translator_test.cpp
)

target_include_directories(llm_client_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_directories(llm_client_tests PRIVATE ${CMAKE_BINARY_DIR}/lib)

target_link_libraries(llm_client_tests
    PRIVATE
        llm_client
        core
        translator
        GTest::GTest
        GTest::Main
        gmock
)

gtest_discover_tests(llm_client_tests)

message(STATUS "Configured tests")
