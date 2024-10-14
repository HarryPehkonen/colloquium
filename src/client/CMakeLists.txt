cmake_minimum_required(VERSION 3.10)

add_library(llm_client
    llm_client.cpp
)

target_include_directories(llm_client
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(llm_client
    PRIVATE
        core
        translator
        http_client
)

set_target_properties(llm_client PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)
