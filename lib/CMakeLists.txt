cmake_minimum_required (VERSION 3.15)


set(OPENSSL_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/openssl-1.1.0f-vs2017)
set(OPENSSL_USE_STATIC_LIBS TRUE)
set(OPENSSL_MSVC_STATIC_RT TRUE)

find_package(OpenSSL REQUIRED)

add_subdirectory(zlib-1.2.11)

set_property(TARGET zlibstatic PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

#add_subdirectory(libssh2)

#set_property(TARGET libssh2 PROPERTY
#             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

#set(LIBSSH2_FOUND TRUE)
#set(LIBSSH2_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/build/lib/libssh2/src ${CMAKE_CURRENT_SOURCE_DIR}/libssh2/include)
#set(LIBSSH2_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libssh2/include)
#set(LIBSSH2_LIBRARY ${PROJECT_SOURCE_DIR}/build/lib/libssh2/src/Release/libssh2.lib)
option(CMAKE_USE_OPENSSL "" On)
option(BUILD_SHARED_LIBS "" Off)

add_subdirectory(curl)

set_property(TARGET libcurl PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_subdirectory(minhook)

set_property(TARGET minhook PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_subdirectory(zydis-master)

set_property(TARGET Zycore PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set_property(TARGET Zydis PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")