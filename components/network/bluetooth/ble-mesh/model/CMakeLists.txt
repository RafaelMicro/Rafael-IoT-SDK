sdk_generate_library(ble_mesh_model)
sdk_add_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/common/include
    ${CMAKE_CURRENT_LIST_DIR}/generics/battery/include
    ${CMAKE_CURRENT_LIST_DIR}/generics/default-trans-time/include
    ${CMAKE_CURRENT_LIST_DIR}/generics/level/include
    ${CMAKE_CURRENT_LIST_DIR}/generics/location/include
    ${CMAKE_CURRENT_LIST_DIR}/generics/onoff/include
    ${CMAKE_CURRENT_LIST_DIR}/generics/power-level/include
    ${CMAKE_CURRENT_LIST_DIR}/generics/power-onoff/include
    ${CMAKE_CURRENT_LIST_DIR}/lighting/ctl/include
    ${CMAKE_CURRENT_LIST_DIR}/lighting/hsl/include
    ${CMAKE_CURRENT_LIST_DIR}/lighting/lightness/include
    ${CMAKE_CURRENT_LIST_DIR}/lighting/xyl/include
    ${CMAKE_CURRENT_LIST_DIR}/scene/include
    ${CMAKE_CURRENT_LIST_DIR}/scheduler/include
    ${CMAKE_CURRENT_LIST_DIR}/time/include
    ${CMAKE_CURRENT_LIST_DIR}/vendor/include

)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_MESH_MODEL_CLIENT ${CMAKE_CURRENT_LIST_DIR}/common/src/mmdl_client.c
)

sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_MESH_MODEL_SERVER ${CMAKE_CURRENT_LIST_DIR}/common/src/mmdl_common.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_MESH_MODEL_GENERICS_BATTERY ${CMAKE_CURRENT_LIST_DIR}/generics/battery/src/mmdl_gen_battery_sr_main.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_MESH_MODEL_GENERICS_DEFAULT_TRANS_TIME ${CMAKE_CURRENT_LIST_DIR}/generics/default-trans-time/src/mmdl_gen_default_trans_time_sr_main.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_MESH_MODEL_GENERICS_LEVEL ${CMAKE_CURRENT_LIST_DIR}/generics/level/src/mmdl_gen_level_sr_main.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_MESH_MODEL_GENERICS_LOCATION ${CMAKE_CURRENT_LIST_DIR}/generics/location/src/mmdl_gen_location_sr_main.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_MESH_MODEL_GENERICS_ONOFF ${CMAKE_CURRENT_LIST_DIR}/generics/onoff/src/mmdl_gen_onoff_cl_main.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_MESH_MODEL_GENERICS_ONOFF ${CMAKE_CURRENT_LIST_DIR}/generics/onoff/src/mmdl_gen_onoff_sr_main.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_MESH_MODEL_GENERICS_POWER_LEVEL ${CMAKE_CURRENT_LIST_DIR}/generics/power-level/src/mmdl_gen_power_level_sr_main.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_MESH_MODEL_GENERICS_POWER_ONOFF ${CMAKE_CURRENT_LIST_DIR}/generics/power-onoff/src/mmdl_gen_power_onoff_sr_main.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_MESH_MODEL_LIGHTING_CTL ${CMAKE_CURRENT_LIST_DIR}/lighting/ctl/src/mmdl_light_ctl_sr_main.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_MESH_MODEL_LIGHTING_CTL ${CMAKE_CURRENT_LIST_DIR}/lighting/ctl/src/mmdl_light_ctl_temperature_sr_main.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_MESH_MODEL_LIGHTING_HSL ${CMAKE_CURRENT_LIST_DIR}/lighting/hsl/src/mmdl_light_hsl_hue_sr_main.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_MESH_MODEL_LIGHTING_HSL ${CMAKE_CURRENT_LIST_DIR}/lighting/hsl/src/mmdl_light_hsl_saturation_sr_main.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_MESH_MODEL_LIGHTING_HSL ${CMAKE_CURRENT_LIST_DIR}/lighting/hsl/src/mmdl_light_hsl_sr_main.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_MESH_MODEL_LIGHTING_LIGHTNESS ${CMAKE_CURRENT_LIST_DIR}/lighting/lightness/src/mmdl_light_lightness_sr_main.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_MESH_MODEL_LIGHTING_XYL ${CMAKE_CURRENT_LIST_DIR}/lighting/xyl/src/mmdl_light_xyl_sr_main.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_MESH_MODEL_SCENE ${CMAKE_CURRENT_LIST_DIR}/scene/src/mmdl_scene_sr_main.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_MESH_MODEL_SCHEDULER ${CMAKE_CURRENT_LIST_DIR}/scheduler/src/mmdl_scheduler_sr_main.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_MESH_MODEL_TIME ${CMAKE_CURRENT_LIST_DIR}/time/src/mmdl_time_sr_main.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_MESH_MODEL_VENDOR ${CMAKE_CURRENT_LIST_DIR}/vendor/src/mmdl_trsp_cl_main.c
)
sdk_library_add_sources_ifdef(
    CONFIG_BLUETOOTH_LE_MESH_MODEL_VENDOR ${CMAKE_CURRENT_LIST_DIR}/vendor/src/mmdl_trsp_sr_main.c
)