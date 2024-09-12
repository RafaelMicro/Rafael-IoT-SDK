sdk_use_app_lib()

if(CONFIG_OT_DEVICE_TYPE_FTD)
    add_definitions(-DOPENTHREAD_FTD)
endif()
if(CONFIG_RADIO_SUBGHZ_OQPSK_SUPPORT)
    if(CONFIG_SUBG_FREQUENCY_BAND_915)
        add_definitions(
            -DCONFIG_APP_SUBG_FREQUENCY_BAND_915=1
            -DCONFIG_APP_SUBG_FREQUENCY=920000
            -DCONFIG_APP_SUBG_FREQUENCY_SPACING=500
            ) 
    elseif(CONFIG_SUBG_FREQUENCY_BAND_868)
        add_definitions(
            -DCONFIG_APP_SUBG_FREQUENCY_BAND_868=1
            -DCONFIG_APP_SUBG_FREQUENCY=863000
            -DCONFIG_APP_SUBG_FREQUENCY_SPACING=500
            )

    elseif(CONFIG_SUBG_FREQUENCY_BAND_470)
        add_definitions(
            -DCONFIG_APP_SUBG_FREQUENCY_BAND_470=1
            -DCONFIG_APP_SUBG_FREQUENCY=470000
            -DCONFIG_APP_SUBG_FREQUENCY_SPACING=2000
            )
    elseif(CONFIG_SUBG_FREQUENCY_BAND_433)
        add_definitions(
            -DCONFIG_APP_SUBG_FREQUENCY_BAND_433=1
            -DCONFIG_APP_SUBG_FREQUENCY=433000
            -DCONFIG_APP_SUBG_FREQUENCY_SPACING=500
            )
    endif()
    add_definitions(
        -DCONFIG_APP_SUBG_
    )
endif()

sdk_add_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/ot-cli-mtd-sleep/Include
    )
target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/ot-cli-mtd-sleep/app_task.c
    ${CMAKE_CURRENT_LIST_DIR}/ot-cli-mtd-sleep/app_udp.c
    ${CMAKE_CURRENT_LIST_DIR}/ot-cli-mtd-sleep/network_management.c
    ${CMAKE_CURRENT_LIST_DIR}/ot-cli-mtd-sleep/cli_uart.cpp
)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/ot-cli-mtd-sleep/main.c)
setup_project(ot-cli-mtd-sleep)