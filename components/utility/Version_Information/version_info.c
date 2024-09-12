#include "version_api.h"

void lib_version_init(void)
{
    volatile version_entry_t *pt_lib_ver = {0};

#ifdef CONFIG_BOOTLOADER
    bootloader_ver_get(pt_lib_ver);
#else
    ble_lib_ver_get(pt_lib_ver);
    ble_mesh_lib_ver_get(pt_lib_ver);
    zigbee_lib_ver_get(pt_lib_ver);
    thread_lib_ver_get(pt_lib_ver);
    matter_lib_ver_get(pt_lib_ver);
#endif
}




