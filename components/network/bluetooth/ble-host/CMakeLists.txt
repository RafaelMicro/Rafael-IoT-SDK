sdk_add_static_library(ble_host)
sdk_add_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/ble-cmd-api/include
    ${CMAKE_CURRENT_LIST_DIR}/hci/include
    ${CMAKE_CURRENT_LIST_DIR}/fota/include
    ${CMAKE_CURRENT_LIST_DIR}/uecc
)
sdk_add_subdirectory_ifdef(CONFIG_BUILD_COMPONENT_BLUETOOTH_LE_HOST ${CMAKE_CURRENT_LIST_DIR}/ble-cmd-api)
sdk_add_subdirectory_ifdef(CONFIG_BUILD_COMPONENT_BLUETOOTH_LE_HOST ${CMAKE_CURRENT_LIST_DIR}/ble-service)
sdk_add_subdirectory_ifdef(CONFIG_BLUETOOTH_LE_SERVICE_FOTA ${CMAKE_CURRENT_LIST_DIR}/fota)