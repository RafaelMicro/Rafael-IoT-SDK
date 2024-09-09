
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/crypto/Include)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/crypto/main.c)
setup_project(crypto)