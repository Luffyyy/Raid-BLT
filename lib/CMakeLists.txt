cmake_minimum_required (VERSION 3.15)

set(OPENSSL_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/openssl-1.1.0f-vs2017)
set(OPENSSL_USE_STATIC_LIBS TRUE)
set(OPENSSL_MSVC_STATIC_RT TRUE)

find_package(OpenSSL REQUIRED)

add_subdirectory(zlib-1.2.11)

set_property(TARGET zlibstatic PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# curl options
# with set all options are not applied at the first configuration
option(CMAKE_USE_OPENSSL "" ON)
option(CMAKE_USE_LIBSSH2 "" OFF)
option(BUILD_SHARED_LIBS "" OFF)
option(CURL_DISABLE_DICT "" ON)
option(CURL_DISABLE_FILE "" ON)
option(CURL_DISABLE_FTP "" ON)
option(CURL_DISABLE_GOPHER "" ON)
option(CURL_DISABLE_IMAP "" ON)
option(CURL_DISABLE_LDAP "" ON)
option(CURL_DISABLE_LDAPS "" ON)
option(CURL_DISABLE_POP3 "" ON)
option(CURL_DISABLE_RTSP "" ON)
option(CURL_DISABLE_SMTP "" ON)
option(CURL_DISABLE_TELNET "" ON)
option(CURL_DISABLE_TFTP "" ON)
option(BUILD_CURL_EXE "" OFF)
option(ENABLE_THREADED_RESOLVER "" ON)
option(BUILD_TESTING "" OFF)

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
