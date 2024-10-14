cmake_minimum_required(VERSION 3.10)

add_library(translator
    openai_translator.cpp
)

target_include_directories(translator
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(translator
    PRIVATE
        core
        nlohmann_json::nlohmann_json
)

set_target_properties(translator PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

message(STATUS "Configured translator library")
