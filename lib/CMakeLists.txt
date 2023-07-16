cmake_minimum_required (VERSION 3.15)


set(OPENSSL_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libssh2)

add_subdirectory(libssh2)

set_property(TARGET libssh2 PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(LIBSSH2_FOUND TRUE)
set(LIBSSH2_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/build/lib/libssh2/src ${CMAKE_CURRENT_SOURCE_DIR}/libssh2/include)
set(LIBSSH2_LIBRARY_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/build/lib/libssh2/src/Debug ${CMAKE_BINARY_DIR}/lib/libssh2/src/Release)
set(LIBSSH2_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libssh2/include)
set(LIBSSH2_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/lib/libssh2/src/Debug)
#set(LIBSSH2_LIBRARY_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/build/lib/libssh2/src/Debug ${CMAKE_CURRENT_SOURCE_DIR}/build/lib/libssh2/src/Release)

add_subdirectory(curl)

set_property(TARGET libcurl PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_subdirectory(minhook)

set_property(TARGET minhook PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_subdirectory(zlib-1.2.11)

set_property(TARGET zlibstatic PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")