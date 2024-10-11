cmake_minimum_required(VERSION 3.10)

project(http_client VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)

add_library(http_client
    src/curl_http_client.cpp
    src/curl_http_client.hpp
    include/http_client/ihttp_client.hpp 
)

target_include_directories(http_client
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(http_client PRIVATE CURL::libcurl)

set_target_properties(http_client PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

install(TARGETS http_client
    EXPORT http_clientTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)

install(EXPORT http_clientTargets
    FILE http_clientTargets.cmake
    NAMESPACE http_client::
    DESTINATION lib/cmake/http_client
)
