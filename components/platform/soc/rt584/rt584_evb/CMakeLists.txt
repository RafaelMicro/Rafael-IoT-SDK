sdk_generate_library(rt584_evb)
sdk_set_linker_script(${CMAKE_CURRENT_LIST_DIR}/evb/ld/rt584.ld)
sdk_add_include_directories(evb/src)
sdk_add_include_directories(include)
add_definitions(-DCHIP_NAME="584" )
sdk_library_add_sources(
    rt_main.c
    evb/src/boot/gcc/startup.c
    evb/src/debug.c
    evb/src/sscanf.c
    evb/src/vsscanf.c
    evb/src/strntoumax.c
)