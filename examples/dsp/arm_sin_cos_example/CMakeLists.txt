sdk_add_subdirectory_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_LIST_DIR}/rtos)

sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

sdk_use_app_lib()
target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/arm_sin_cos_example_f32.c
)

# Turn ARM's example main() into a callable dsp_example_run() and enable its
# printf output. The ARM source itself is kept unmodified.
set_source_files_properties(
    ${CMAKE_CURRENT_LIST_DIR}/src/arm_sin_cos_example_f32.c
    PROPERTIES COMPILE_DEFINITIONS "main=dsp_example_run;SEMIHOSTING"
)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/src/main.c)
setup_project(arm_sin_cos_example)
