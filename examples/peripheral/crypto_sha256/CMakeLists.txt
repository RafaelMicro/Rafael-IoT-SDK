
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/crypto_sha256/Include)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/crypto_sha256/main.c)
setup_project(crypto_sha256)