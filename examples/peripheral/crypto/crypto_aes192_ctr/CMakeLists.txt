sdk_add_subdirectory_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_LIST_DIR}/rtos)
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/crypto_aes192_ctr/Include)
sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/crypto_aes192_ctr/main.c)
setup_project(crypto_aes192_ctr)
