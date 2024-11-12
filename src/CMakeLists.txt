cmake_minimum_required(VERSION 3.10)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/http_client/include
)

add_subdirectory(core)
add_subdirectory(http_client)
add_subdirectory(translator)
add_subdirectory(client)
add_subdirectory(session)
