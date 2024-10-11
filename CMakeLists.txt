cmake_minimum_required(VERSION 3.10)
project(llm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(src/http_client)
add_subdirectory(tests/http_client)
