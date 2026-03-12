message("=== Configuring mesh-it-up: FTD ===")

# ========================================
# 1. Base setting
# ========================================
sdk_add_subdirectory_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_LIST_DIR}/rtos)
sdk_use_app_lib()

if(CONFIG_MIU_DEVICE_TYPE_FTD)
    add_definitions(-DOPENTHREAD_FTD)
endif()

# ========================================
# 3. Application layer setting
# ========================================
add_definitions(
    -DCONFIG_APP_TASK_UDP_LISTEN_PORT=5678
    -DCONFIG_APP_TASK_CONTROL_CMD_ENABLE=1
    -DCONFIG_APP_TASK_CENTRAL_ENABLE=1
    -DCONFIG_APP_TASK_OTA_ENABLE=1
)

if (CONFIG_BUILD_COMPONENT_ENHANCED_FLASH_DATASET)
    if(CONFIG_FLASHCTRL_SECURE_EN)
        sdk_add_compile_options(
            -DEFD_START_ADDR=0x100F0000
            -DENV_AREA_SIZE=0x4000
        )
    else()
        sdk_add_compile_options(
            -DEFD_START_ADDR=0xF0000
            -DENV_AREA_SIZE=0x4000
        )
    endif()
endif()

set(MIU_COMMON_DIR "../miu-common")
set(MIU_COMPONENTS_DIR "${CMAKE_SOURCE_DIR}/components/network/mesh-it-up")

sdk_add_include_directories(
    ${MIU_COMMON_DIR}/include
    ${MIU_COMPONENTS_DIR}/miu_port/include
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/crypto_aes128_encrypt/Include
)

# ========================================
# 4. Source Files
# ========================================
target_sources(app PRIVATE
    ${MIU_COMMON_DIR}/src/app_led.c
    ${MIU_COMMON_DIR}/src/app_udp.c
    ${MIU_COMMON_DIR}/src/app_uart.c
    ${MIU_COMMON_DIR}/src/app_ota.c
    ${MIU_COMMON_DIR}/src/app_mac_raw.c
    ${MIU_COMMON_DIR}/src/cli_uart.cpp
    ${MIU_COMMON_DIR}/src/app_control_cmd.c
    ${CMAKE_CURRENT_LIST_DIR}/src/app_net_mgm.c
    ${CMAKE_CURRENT_LIST_DIR}/src/app_task.c
    ${CMAKE_CURRENT_LIST_DIR}/src/ota_download_cmd_handler.c
)

# ========================================
# 5. Main Entry
# ========================================
sdk_set_main_file(${MIU_COMMON_DIR}/src/main.c)

# ========================================
# 6. Project
# ========================================
setup_project(miu-ftd)

# ========================================
# 7. Link Static Library (miu_port_lib.a)
# ========================================
target_link_libraries(app miu_port_lib)