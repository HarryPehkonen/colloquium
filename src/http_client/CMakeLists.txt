cmake_minimum_required(VERSION 3.10)

find_package(CURL REQUIRED)

add_library(http_client
    src/curl_http_client.cpp
)

target_include_directories(http_client
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(http_client
    PRIVATE
        CURL::libcurl
        spdlog::spdlog
)

set_target_properties(http_client PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)
