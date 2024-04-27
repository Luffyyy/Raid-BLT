cmake_minimum_required (VERSION 3.15)

set(OPENSSL_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/openssl-1.1.0f-vs2017)
set(OPENSSL_USE_STATIC_LIBS TRUE)
set(OPENSSL_MSVC_STATIC_RT TRUE)

find_package(OpenSSL REQUIRED)

add_subdirectory(zlib-1.2.11)

set_property(TARGET zlibstatic PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# curl options
option(CMAKE_USE_OPENSSL "" ON)
option(BUILD_SHARED_LIBS "" OFF)
set(CURL_DISABLE_DICT "" ON)
set(CURL_DISABLE_FILE "" ON)
set(CURL_DISABLE_FTP "" ON)
set(CURL_DISABLE_GOPHER "" ON)
set(CURL_DISABLE_IMAP "" ON)
set(CURL_DISABLE_LDAP "" ON)
set(CURL_DISABLE_LDAPS "" ON)
set(CURL_DISABLE_POP3 "" ON)
set(CURL_DISABLE_RTSP "" ON)
set(CURL_DISABLE_SMTP "" ON)
set(CURL_DISABLE_TELNET "" ON)
set(CURL_DISABLE_TFTP "" ON)
set(BUILD_CURL_EXE "" OFF)
set(ENABLE_THREADED_RESOLVER "" ON)

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
