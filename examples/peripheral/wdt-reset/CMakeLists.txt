
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/wdt-reset/include)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/wdt-reset/main.c)

setup_project(wdt-reset)