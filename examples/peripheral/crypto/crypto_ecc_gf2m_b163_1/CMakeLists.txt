sdk_add_subdirectory_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_LIST_DIR}/rtos)
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/crypto_ecc_gf2m_b163_1/Include)
sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/crypto_ecc_gf2m_b163_1/main.c)
setup_project(crypto_ecc_gf2m_b163_1)
