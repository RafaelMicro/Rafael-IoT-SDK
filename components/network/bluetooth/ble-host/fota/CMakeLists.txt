sdk_add_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/include
)
sdk_generate_library(ble_fota)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_FOTA ${CMAKE_CURRENT_LIST_DIR}/src/ble_fota.c
)
