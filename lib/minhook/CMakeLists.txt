cmake_minimum_required (VERSION 3.15)

set(MINHOOK_HEADERS
    include/MinHook.h
)

set(MINHOOK_SOURCES
    src/hde/hde32.c
    src/hde/hde32.h
    src/hde/hde64.c
    src/hde/hde64.h
    src/hde/pstdint.h
    src/hde/table32.h
    src/hde/table64.h
    src/buffer.c
    src/buffer.h
    src/hook.c
    src/trampoline.c
    src/trampoline.h
)

add_library(minhook STATIC ${MINHOOK_HEADERS} ${MINHOOK_SOURCES})

target_link_libraries(minhook PUBLIC Ws2_32 libcryptoMTd libsslMTd Wldap32)

target_include_directories(minhook PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../extern/DirectX SDK/Include)