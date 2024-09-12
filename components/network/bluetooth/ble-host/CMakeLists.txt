sdk_add_static_library(ble_host)
sdk_add_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/ble-cmd-api/include
    ${CMAKE_CURRENT_LIST_DIR}/fota/include
)
sdk_library_add_sources(
    ${CMAKE_CURRENT_LIST_DIR}/ble-cmd-api/src/ble_cmd_advertising.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-cmd-api/src/ble_cmd_att_gatt.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-cmd-api/src/ble_cmd_common.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-cmd-api/src/ble_cmd_connect_cte.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-cmd-api/src/ble_cmd_gap.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-cmd-api/src/ble_cmd_privacy.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-cmd-api/src/ble_cmd_scan.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-cmd-api/src/ble_cmd_security_manager.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-cmd-api/src/ble_host_ref.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_FOTA ${CMAKE_CURRENT_LIST_DIR}/fota/src/ble_fota.c
)

sdk_add_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/ble-service/bas/include
    ${CMAKE_CURRENT_LIST_DIR}/ble-service/bls/include
    ${CMAKE_CURRENT_LIST_DIR}/ble-service/common/include
    ${CMAKE_CURRENT_LIST_DIR}/ble-service/dis/include
    ${CMAKE_CURRENT_LIST_DIR}/ble-service/fota/include
    ${CMAKE_CURRENT_LIST_DIR}/ble-service/gaps/include
    ${CMAKE_CURRENT_LIST_DIR}/ble-service/gatts/include
    ${CMAKE_CURRENT_LIST_DIR}/ble-service/hids/include
    ${CMAKE_CURRENT_LIST_DIR}/ble-service/hrs/include
    ${CMAKE_CURRENT_LIST_DIR}/ble-service/mesh_prov/include
    ${CMAKE_CURRENT_LIST_DIR}/ble-service/mesh_proxy/include
    ${CMAKE_CURRENT_LIST_DIR}/ble-service/trsps/include
)
sdk_library_add_sources(
    ${CMAKE_CURRENT_LIST_DIR}/ble-service/common/src/ble_service_common.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_BAS ${CMAKE_CURRENT_LIST_DIR}/ble-service/bas/src/ble_service_bas.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_BLS ${CMAKE_CURRENT_LIST_DIR}/ble-service/bls/src/ble_service_bls.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_DIS ${CMAKE_CURRENT_LIST_DIR}/ble-service/dis/src/ble_service_dis.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_FOTA ${CMAKE_CURRENT_LIST_DIR}/ble-service/fota/src/ble_service_fotas.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_GAP ${CMAKE_CURRENT_LIST_DIR}/ble-service/gaps/src/ble_service_gaps.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_GATT ${CMAKE_CURRENT_LIST_DIR}/ble-service/gatts/src/ble_service_gatts.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_HID ${CMAKE_CURRENT_LIST_DIR}/ble-service/hids/src/ble_service_hids.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_HRS ${CMAKE_CURRENT_LIST_DIR}/ble-service/hrs/src/ble_service_hrs.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_MESH ${CMAKE_CURRENT_LIST_DIR}/ble-service/mesh_prov/src/ble_service_mesh_prov.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_MESH ${CMAKE_CURRENT_LIST_DIR}/ble-service/mesh_proxy/src/ble_service_mesh_proxy.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_SERVICE_TRSP ${CMAKE_CURRENT_LIST_DIR}/ble-service/trsps/src/ble_service_trsps.c
)