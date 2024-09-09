
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/gpio-output/include)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/gpio-output/main.c)

setup_project(gpio-output)