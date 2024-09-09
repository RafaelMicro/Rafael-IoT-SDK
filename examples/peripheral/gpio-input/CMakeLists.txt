
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/gpio-input/include)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/gpio-input/main.c)

setup_project(gpio-input)