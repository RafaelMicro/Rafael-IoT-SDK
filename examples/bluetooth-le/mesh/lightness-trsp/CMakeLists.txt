
sdk_add_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/lightness-trsp/include
    ${CMAKE_CURRENT_LIST_DIR}/ble-mesh-element/include
    ${CMAKE_CURRENT_LIST_DIR}/ble-app-profile/include
)
sdk_use_app_lib()
target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/ble-app-profile/src/ble_profile_def.c
    ${CMAKE_CURRENT_LIST_DIR}/ble-mesh-element/src/ble_mesh_element_def.c
    ${CMAKE_CURRENT_LIST_DIR}/lightness-trsp/src/mesh_mdl_handler.c
)
sdk_set_main_file(
    ${CMAKE_CURRENT_LIST_DIR}/lightness-trsp/src/main.c

)

sdk_add_compile_options(-DBLE_SUPPORT_NUM_CONN_MAX=${CONFIG_BLE_SUPPORT_NUM_CONN_MAX} -DMESH_MODEL_SERVER=1)
setup_project(lightness-trsp)