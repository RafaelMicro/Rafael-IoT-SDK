sdk_add_subdirectory_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_LIST_DIR}/rtos)

sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

sdk_use_app_lib()
target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/arm_graphic_equalizer_example_q31.c
    ${CMAKE_CURRENT_LIST_DIR}/src/arm_graphic_equalizer_data.c
    ${CMAKE_CURRENT_LIST_DIR}/src/math_helper.c
)

# Turn ARM's example main() into a callable dsp_example_run() and enable its
# printf output. The ARM source itself is kept unmodified. (This is the only
# fixed-point (q31) example in the set.)
set_source_files_properties(
    ${CMAKE_CURRENT_LIST_DIR}/src/arm_graphic_equalizer_example_q31.c
    PROPERTIES COMPILE_DEFINITIONS "main=dsp_example_run;SEMIHOSTING"
)

sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/src/main.c)
setup_project(arm_graphic_equalizer_example)
