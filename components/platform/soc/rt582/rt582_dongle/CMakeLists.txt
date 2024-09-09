

sdk_generate_library(rt582_dongle)
sdk_set_linker_script(${CMAKE_CURRENT_LIST_DIR}/dongle/ld/rt582.ld)
add_definitions(-DCHIP_NAME="582" -DCONFIG_TARGET_BOARD="dongle")
sdk_add_include_directories(dongle/src)
sdk_library_add_sources(
    rt_main.c
    dongle/src/boot/gcc/startup.S
    dongle/src/debug.c
    dongle/src/memmove.c
    dongle/src/sscanf.c
    dongle/src/strntoumax.c
    dongle/src/vsscanf.c
)

