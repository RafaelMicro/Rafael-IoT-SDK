
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/crypto_pbkdf2/Include)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/crypto_pbkdf2/main.c)
setup_project(crypto_pbkdf2)