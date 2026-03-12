sdk_add_subdirectory_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_LIST_DIR}/rtos)
sdk_add_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/rf-ev-15p4/Include
    ${CMAKE_CURRENT_LIST_DIR}/rf-ev-15p4/RFB_2p4G/Include
)
sdk_use_app_lib()
target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/rf-ev-15p4/RFB_2p4G/mac_frame_gen.c
    ${CMAKE_CURRENT_LIST_DIR}/rf-ev-15p4/RFB_2p4G/rfb_sample.c
)
sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/rf-ev-15p4/main.c)
setup_project(rf-ev-15p4)
