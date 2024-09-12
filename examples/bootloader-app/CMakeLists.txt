
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/bootloader-app/include)

sdk_use_app_lib()
target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/bootloader-app/bootloader.c
    ${CMAKE_CURRENT_LIST_DIR}/bootloader-app/slip.c
    ${CMAKE_CURRENT_LIST_DIR}/bootloader-app/isp.c
)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/bootloader-app/main.c)
setup_project(bootloader-app)