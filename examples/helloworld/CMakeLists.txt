
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/helloworld/Include)

sdk_use_app_lib()
target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/helloworld/app.c
)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/helloworld/main.c)
setup_project(helloworld)