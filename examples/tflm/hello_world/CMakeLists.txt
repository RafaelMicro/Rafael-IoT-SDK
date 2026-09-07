sdk_add_subdirectory_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_LIST_DIR}/rtos)

sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

sdk_use_app_lib()
set(TFLM_APP_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/hello_world_run.cc
    ${CMAKE_CURRENT_LIST_DIR}/src/hello_world_int8_model_data.cc
    ${CMAKE_CURRENT_LIST_DIR}/src/tflm_port.cc
)
target_sources(app PRIVATE ${TFLM_APP_SOURCES})

# The app C++ units include TFLM headers, which require C++17 (the SDK global
# C++ standard is c++11; the later -std wins). -fno-threadsafe-statics matches
# how the tflm library is built and avoids __cxa_guard references. (Hosted
# libstdc++ is ensured by the C++-scoped -ffreestanding in compiler_flags.cmake.)
target_compile_options(app PRIVATE -std=c++17 -fno-threadsafe-statics)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/src/main.c)
setup_project(hello_world)

# The executable pulls in C++ objects (TFLM/app), so CMake would otherwise link
# with the g++ driver, whose CMAKE_CXX_FLAGS carry -nostdlib (toolchain.cmake) -
# that drops the C runtime startup (crt0 -> _start) and breaks the link. Force
# the C driver: it provides crt0/_start via -specs=nano/nosys, and no libstdc++
# is needed here (-fno-exceptions/-fno-rtti; operator new/delete and the __cxa
# shims are supplied by tflm_port.cc).
set_target_properties(hello_world_${CONFIG_CHIP}.elf PROPERTIES LINKER_LANGUAGE C)
