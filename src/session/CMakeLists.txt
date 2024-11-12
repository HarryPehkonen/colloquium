cmake_minimum_required(VERSION 3.10)

add_library(session
    llm_session.cpp
)

# include translators
target_include_directories(session
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
        ${async_deque_SOURCE_DIR}/include
)

target_link_libraries(session
    PRIVATE
        core
        translator
        http_client
        spdlog::spdlog
        async_deque
)

set_target_properties(session PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

message(STATUS "Configured session module")
message(STATUS ${CMAKE_SOURCE_DIR})
