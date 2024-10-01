cmake_minimum_required (VERSION 3.15)

set(LUA51_SOURCES
    luajit/src/lib_aux.c
    luajit/src/lib_base.c
    luajit/src/lib_bit.c
    luajit/src/lib_buffer.c
    luajit/src/lib_debug.c
    luajit/src/lib_ffi.c
    luajit/src/lib_init.c
    luajit/src/lib_io.c
    luajit/src/lib_jit.c
    luajit/src/lib_math.c
    luajit/src/lib_os.c
    luajit/src/lib_package.c
    luajit/src/lib_string.c
    luajit/src/lib_table.c
    luajit/src/lj_alloc.c
    luajit/src/lj_api.c
    luajit/src/lj_asm.c
    luajit/src/lj_assert.c
    luajit/src/lj_bc.c
    luajit/src/lj_bcread.c
    luajit/src/lj_bcwrite.c
    luajit/src/lj_buf.c
    luajit/src/lj_carith.c
    luajit/src/lj_ccall.c
    luajit/src/lj_ccallback.c
    luajit/src/lj_cconv.c
    luajit/src/lj_cdata.c
    luajit/src/lj_char.c
    luajit/src/lj_clib.c
    luajit/src/lj_cparse.c
    luajit/src/lj_crecord.c
    luajit/src/lj_ctype.c
    luajit/src/lj_debug.c
    luajit/src/lj_dispatch.c
    luajit/src/lj_err.c
    luajit/src/lj_ffrecord.c
    luajit/src/lj_func.c
    luajit/src/lj_gc.c
    luajit/src/lj_gdbjit.c
    luajit/src/lj_ir.c
    luajit/src/lj_lex.c
    luajit/src/lj_lib.c
    luajit/src/lj_load.c
    luajit/src/lj_mcode.c
    luajit/src/lj_meta.c
    luajit/src/lj_obj.c
    luajit/src/lj_opt_dce.c
    luajit/src/lj_opt_fold.c
    luajit/src/lj_opt_loop.c
    luajit/src/lj_opt_mem.c
    luajit/src/lj_opt_narrow.c
    luajit/src/lj_opt_sink.c
    luajit/src/lj_opt_split.c
    luajit/src/lj_parse.c
    luajit/src/lj_prng.c
    luajit/src/lj_profile.c
    luajit/src/lj_record.c
    luajit/src/lj_serialize.c
    luajit/src/lj_snap.c
    luajit/src/lj_state.c
    luajit/src/lj_str.c
    luajit/src/lj_strfmt.c
    luajit/src/lj_strfmt_num.c
    luajit/src/lj_strscan.c
    luajit/src/lj_tab.c
    luajit/src/lj_trace.c
    luajit/src/lj_udata.c
    luajit/src/lj_vmevent.c
    luajit/src/lj_vmmath.c

    lj_vm.obj
)

set(LUA51_BUILDVM_SOURCES
    luajit/src/host/buildvm.c
    luajit/src/host/buildvm_asm.c
    luajit/src/host/buildvm_peobj.c
    luajit/src/host/buildvm_fold.c
    luajit/src/host/buildvm_lib.c
)

set(LUAJIT_SOURCES
    luajit/src/luajit.c
)

add_library(lua51 STATIC ${LUA51_SOURCES})
add_library(lua51-objects OBJECT ${LUA51_SOURCES})
add_library(lua51-shared SHARED ${LUA51_SOURCES})
add_executable(LuaBuildVM ${LUA51_BUILDVM_SOURCES})

add_executable(luajit ${LUAJIT_SOURCES})

target_link_libraries(luajit PUBLIC lua51)

target_compile_definitions(lua51-objects PUBLIC LUA_BUILD_AS_DLL)

target_include_directories(lua51 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/luajit/src)
target_include_directories(lua51-shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/luajit/src)
target_include_directories(lua51-objects PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/luajit/src)
target_include_directories(LuaBuildVM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/luajit/src)
target_include_directories(luajit PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/luajit/src)

target_compile_definitions(lua51-shared PUBLIC LUA_BUILD_AS_DLL)

set_property(TARGET lua51 PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set_property(TARGET lua51 PROPERTY CXX_STANDARD 20)

set_property(TARGET lua51-shared PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set_property(TARGET lua51-shared PROPERTY CXX_STANDARD 20)

set_property(TARGET lua51-objects PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set_property(TARGET lua51-objects PROPERTY CXX_STANDARD 20)

set_property(TARGET LuaBuildVM PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set_property(TARGET LuaBuildVM PROPERTY CXX_STANDARD 20)

set_property(TARGET luajit PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set_property(TARGET luajit PROPERTY CXX_STANDARD 20)