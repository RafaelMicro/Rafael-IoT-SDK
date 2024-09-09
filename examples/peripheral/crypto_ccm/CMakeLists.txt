
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/crypto_ccm/Include)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/crypto_ccm/main.c)
setup_project(crypto_ccm)