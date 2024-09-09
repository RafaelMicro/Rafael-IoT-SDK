
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/crypto_ecjpake/Include)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/crypto_ecjpake/main.c)
setup_project(crypto_ecjpake)