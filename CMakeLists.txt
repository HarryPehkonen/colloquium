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
        nlohmann_json::nlohmann_json
        spdlog::spdlog
)

# Set the output directory for the main application
set_target_properties(llm_client_app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
