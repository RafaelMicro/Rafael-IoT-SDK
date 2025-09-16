sdk_add_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/bas/include
    ${CMAKE_CURRENT_LIST_DIR}/bls/include
    ${CMAKE_CURRENT_LIST_DIR}/common/include
    ${CMAKE_CURRENT_LIST_DIR}/dis/include
    ${CMAKE_CURRENT_LIST_DIR}/fota/include
    ${CMAKE_CURRENT_LIST_DIR}/gaps/include
    ${CMAKE_CURRENT_LIST_DIR}/gatts/include
    ${CMAKE_CURRENT_LIST_DIR}/hids/include
    ${CMAKE_CURRENT_LIST_DIR}/hrs/include
    ${CMAKE_CURRENT_LIST_DIR}/mesh_prov/include
    ${CMAKE_CURRENT_LIST_DIR}/mesh_proxy/include
    ${CMAKE_CURRENT_LIST_DIR}/trsps/include
    ${CMAKE_CURRENT_LIST_DIR}/matter/include
    ${CMAKE_CURRENT_LIST_DIR}/esls/include
    ${CMAKE_CURRENT_LIST_DIR}/ots/include
)
sdk_generate_library(ble_service)
sdk_library_add_sources(
    ${CMAKE_CURRENT_LIST_DIR}/common/src/ble_service_common.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_BAS ${CMAKE_CURRENT_LIST_DIR}/bas/src/ble_service_bas.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_BLS ${CMAKE_CURRENT_LIST_DIR}/bls/src/ble_service_bls.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_DIS ${CMAKE_CURRENT_LIST_DIR}/dis/src/ble_service_dis.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_FOTA ${CMAKE_CURRENT_LIST_DIR}/fota/src/ble_service_fotas.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_GAP ${CMAKE_CURRENT_LIST_DIR}/gaps/src/ble_service_gaps.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_GATT ${CMAKE_CURRENT_LIST_DIR}/gatts/src/ble_service_gatts.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_HID ${CMAKE_CURRENT_LIST_DIR}/hids/src/ble_service_hids.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_HRS ${CMAKE_CURRENT_LIST_DIR}/hrs/src/ble_service_hrs.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_MESH ${CMAKE_CURRENT_LIST_DIR}/mesh_prov/src/ble_service_mesh_prov.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_MESH ${CMAKE_CURRENT_LIST_DIR}/mesh_proxy/src/ble_service_mesh_proxy.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_TRSP ${CMAKE_CURRENT_LIST_DIR}/trsps/src/ble_service_trsps.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_MATTER ${CMAKE_CURRENT_LIST_DIR}/matter/src/ble_service_matter.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_ESLS ${CMAKE_CURRENT_LIST_DIR}/esls/src/ble_service_esls.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_OTS ${CMAKE_CURRENT_LIST_DIR}/ots/src/ble_service_ots.c
)