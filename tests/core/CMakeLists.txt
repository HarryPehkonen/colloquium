cmake_minimum_required(VERSION 3.10)
project(ParameterTest)

set(CMAKE_CXX_STANDARD 14)

# Find Google Test
find_package(GTest REQUIRED)

# Add the parameter library
add_library(parameter STATIC parameter.cpp parameter.hpp)

# Add the test executable
add_executable(parameter_test parameter_test.cpp)

# Link the test executable with Google Test and the parameter library
target_link_libraries(parameter_test PRIVATE GTest::GTest GTest::Main parameter)

# Add include directories
target_include_directories(parameter_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Add the test
add_test(NAME ParameterTest COMMAND parameter_test)
