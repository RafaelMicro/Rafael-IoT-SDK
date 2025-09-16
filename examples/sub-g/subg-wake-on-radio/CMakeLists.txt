sdk_add_subdirectory_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_LIST_DIR}/rtos)
sdk_add_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/subg-wake-on-radio/Include
)
sdk_use_app_lib()
target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/subg-wake-on-radio/mac_frame_gen.c
)
sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/subg-wake-on-radio/main.c)
if(CONFIG_HOSAL_SOC_IDLE_SLEEP)
    setup_project(subg-wake-on-radio-sleep)
else()
    setup_project(subg-wake-on-radio-tx)
endif()
