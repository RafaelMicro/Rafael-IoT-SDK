sdk_generate_component_tag()

if(CONFIG_BOOTLOADER)

else()
if(${CHIP_RF} STREQUAL "RT569")
    sdk_add_subdirectory_ifdef(CONFIG_BUILD_COMPONENT_RT569_RF ${CMAKE_CURRENT_LIST_DIR}/rt569-rf)
    sdk_add_subdirectory_ifdef(CONFIG_BUILD_COMPONENT_RT569_FW ${CMAKE_CURRENT_LIST_DIR}/rt569-fw)
    sdk_add_subdirectory_ifdef(CONFIG_BUILD_COMPONENT_CSS ${CMAKE_CURRENT_LIST_DIR}/css)
endif()


sdk_add_subdirectory_ifdef(CONFIG_BUILD_COMPONENT_RUCI ${CMAKE_CURRENT_LIST_DIR}/ruci)
sdk_add_subdirectory_ifdef(CONFIG_BUILD_COMPONENT_LMAC5P4 ${CMAKE_CURRENT_LIST_DIR}/lmac15p4)
sdk_add_subdirectory_ifdef(CONFIG_BUILD_COMPONENT_BLUETOOTH ${CMAKE_CURRENT_LIST_DIR}/bluetooth)
sdk_add_subdirectory_ifdef(CONFIG_BUILD_COMPONENT_ZIGBEE ${CMAKE_CURRENT_LIST_DIR}/zigbee)
sdk_add_subdirectory_ifdef(CONFIG_BUILD_COMPONENT_SUBG_CTRL ${CMAKE_CURRENT_LIST_DIR}/subg_ctrl)
sdk_add_subdirectory_ifdef(CONFIG_BUILD_COMPONENT_OPENTHREAD ${CMAKE_CURRENT_LIST_DIR}/thread)
sdk_add_subdirectory_ifdef(CONFIG_BUILD_COMPONENT_MESH_IT_UP ${CMAKE_CURRENT_LIST_DIR}/mesh-it-up)
endif()