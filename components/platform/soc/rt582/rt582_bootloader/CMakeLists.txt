sdk_generate_library(rt582_bootloader)
sdk_set_linker_script(${CMAKE_CURRENT_LIST_DIR}/bootloader/ld/rt582.ld)
sdk_add_include_directories(bootloader/src)
sdk_ifndef(CHIP_NAME "582")
sdk_ifndef(CONFIG_TARGET_BOARD "bootloader")
sdk_add_compile_definitions(-D__STARTUP_CLEAR_BSS)

if(CONFIG_BOOTLOADER_DEBUG)
sdk_library_add_sources(
    rt_main.c
    bootloader/src/boot/gcc/startup.S
    bootloader/src/debug.c
    bootloader/src/sscanf.c
    bootloader/src/vsscanf.c
    bootloader/src/strntoumax.c
)
else()
sdk_library_add_sources(
    rt_main.c
    bootloader/src/boot/gcc/startup.S
)
endif()
