cmake_minimum_required(VERSION 3.10)

add_library(core
    message.cpp
    parameter.cpp
    tool.cpp
)

target_include_directories(core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# If core depends on any external libraries, add them here
# target_link_libraries(core PRIVATE some_dependency)

set_target_properties(core PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)
