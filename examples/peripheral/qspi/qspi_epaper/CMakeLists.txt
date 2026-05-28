sdk_add_subdirectory_ifdef(CONFIG_FREERTOS ${CMAKE_CURRENT_LIST_DIR}/rtos)
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/qspi_epaper/Include)
sdk_use_app_lib()
target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/qspi_epaper/Examples/EPD_1in54_test.c
    ${CMAKE_CURRENT_LIST_DIR}/qspi_epaper/Examples/ImageData.c
    ${CMAKE_CURRENT_LIST_DIR}/qspi_epaper/Examples/DEV_Config.c
    ${CMAKE_CURRENT_LIST_DIR}/qspi_epaper/e-Paper/EPD_1in54.c
    ${CMAKE_CURRENT_LIST_DIR}/qspi_epaper/Fonts/font8.c
    ${CMAKE_CURRENT_LIST_DIR}/qspi_epaper/Fonts/font12.c
    ${CMAKE_CURRENT_LIST_DIR}/qspi_epaper/Fonts/font16.c
    ${CMAKE_CURRENT_LIST_DIR}/qspi_epaper/Fonts/font20.c
    ${CMAKE_CURRENT_LIST_DIR}/qspi_epaper/Fonts/font24.c
    ${CMAKE_CURRENT_LIST_DIR}/qspi_epaper/GUI/GUI_Paint.c
)
sdk_set_main_file(${CMAKE_CURRENT_LIST_DIR}/qspi_epaper/main.c)
setup_project(qspi_epaper)
