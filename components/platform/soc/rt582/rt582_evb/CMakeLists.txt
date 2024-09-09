sdk_generate_library(rt582_evb)
sdk_set_linker_script(${CMAKE_CURRENT_LIST_DIR}/evb/ld/rt582.ld)
sdk_add_include_directories(evb/src)
add_definitions(-DCHIP_NAME="582" -DCONFIG_TARGET_BOARD="evb")
sdk_library_add_sources(
    rt_main.c
    evb/src/boot/gcc/startup.S
    evb/src/debug.c
    evb/src/sscanf.c
    evb/src/vsscanf.c
    evb/src/strntoumax.c
)