cmake_minimum_required (VERSION 3.15)

set(IPHLPAPI_SOURCES
    InitState.h
    InitiateState.cpp
    iphp.cpp
    console/console.h
    console/console.cpp
    http/http.h
    http/http.cpp
    signatures/signatures.h
    signatures/signatures.cpp
    threading/queue.h
    threading/threadqueue.cpp
    util/compression.cpp
    util/files.cpp
    util/logging.cpp
    util/util.cpp
    util/util.h
)

message("DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

add_library(IPHLPAPI SHARED ${IPHLPAPI_SOURCES})

target_link_libraries(IPHLPAPI PUBLIC Ws2_32 libcryptoMTd libsslMTd Wldap32 libcurl zlibstatic minhook libssh2)

target_include_directories(IPHLPAPI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../lib/openssl-1.1.0f-vs2017/Include64 
                ${CMAKE_CURRENT_SOURCE_DIR}/../lib/minhook/include ${CMAKE_CURRENT_SOURCE_DIR}/../lib/zlib-1.2.11 ${CMAKE_BINARY_DIR}/lib/zlib-1.2.11)

target_link_directories(IPHLPAPI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../lib/openssl-1.1.0f-vs2017/lib64)

set_property(TARGET IPHLPAPI PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set_target_properties(IPHLPAPI PROPERTIES LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/../proj/iphp.def\"")