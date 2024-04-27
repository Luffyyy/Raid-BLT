cmake_minimum_required (VERSION 3.15)
cmake_policy(SET CMP0091 NEW)

message("${CMAKE_CURRENT_SOURCE_DIR}/CMake")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake;${CMAKE_MODULE_PATH}")

project("IPHLPAPI")

add_subdirectory(lib)
add_subdirectory(src)
