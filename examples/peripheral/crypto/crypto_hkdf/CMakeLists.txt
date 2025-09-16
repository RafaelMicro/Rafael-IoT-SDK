sdk_add_subdirectory_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_LIST_DIR}/rtos)
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/crypto_hkdf/Include)
sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/crypto_hkdf/main.c)
setup_project(crypto_hkdf)
