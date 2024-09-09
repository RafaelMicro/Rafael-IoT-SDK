
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/wdt-interrupt/include)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/wdt-interrupt/main.c)

setup_project(wdt-interrupt)