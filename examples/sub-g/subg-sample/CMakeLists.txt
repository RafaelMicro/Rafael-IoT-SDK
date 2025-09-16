sdk_add_subdirectory_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_LIST_DIR}/rtos)
sdk_add_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/subg-sample/Include
)

sdk_use_app_lib()
target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/subg-sample/mac_frame_gen.c
)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/subg-sample/main.c)
setup_project(subg-sample)