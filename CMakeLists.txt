cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)

project(LLMClientProject VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Find packages
find_package(CURL REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

# Include FetchContent module
include(FetchContent)

# Fetch spdlog
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.x
)
FetchContent_MakeAvailable(spdlog)

include(FetchContent)
FetchContent_Declare(
    async_deque
    GIT_REPOSITORY https://github.com/HarryPehkonen/AsyncDeque.git
    GIT_TAG main  # or specific tag/commit
)
FetchContent_GetProperties(async_deque)
if(NOT async_deque_POPULATED)
    FetchContent_Populate(async_deque)
endif()

# Create interface library for AsyncDeque
add_library(async_deque INTERFACE)
target_include_directories(async_deque INTERFACE ${async_deque_SOURCE_DIR}/include)

#target_link_libraries(your_target PRIVATE async_deque::async_deque)
# Add subdirectories
add_subdirectory(src)
add_subdirectory(tests)

# Add the main application
add_executable(llm_client_app src/main/main.cpp)
target_link_libraries(llm_client_app
    PRIVATE
        llm_client
        http_client
        core
        translator
        session
        nlohmann_json::nlohmann_json
        spdlog::spdlog
        async_deque
)

# Set the output directory for the main application
set_target_properties(llm_client_app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
