cmake_minimum_required(VERSION 3.15)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/entry.cmake)

project(${CONFIG_BUILD_PORJECT} LANGUAGES C CXX ASM)
ENABLE_LANGUAGE(C CXX ASM)
include(config/proj_default.conf)

show_banner()
sdk_add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/components)


sdk_add_subdirectory_ifdef(CONFIG_BASIC_EXAMPLE ${CMAKE_CURRENT_LIST_DIR}/examples/${CONFIG_BUILD_PORJECT})
sdk_add_subdirectory_ifdef(CONFIG_PERIPHERAL_EXAMPLE ${CMAKE_CURRENT_LIST_DIR}/examples/peripheral/${CONFIG_BUILD_PORJECT})
sdk_add_subdirectory_ifdef(CONFIG_BLE_PERIPHERAL_ROLE ${CMAKE_CURRENT_LIST_DIR}/examples/bluetooth-le/peripheral/${CONFIG_BUILD_PORJECT})
sdk_add_subdirectory_ifdef(CONFIG_BLE_CENTRAL_ROLE ${CMAKE_CURRENT_LIST_DIR}/examples/bluetooth-le/central/${CONFIG_BUILD_PORJECT})
sdk_add_subdirectory_ifdef(CONFIG_BLE_MULTI_LINK ${CMAKE_CURRENT_LIST_DIR}/examples/bluetooth-le/multi-link/${CONFIG_BUILD_PORJECT})
sdk_add_subdirectory_ifdef(CONFIG_BLE_OTHERS ${CMAKE_CURRENT_LIST_DIR}/examples/bluetooth-le/others/${CONFIG_BUILD_PORJECT})
sdk_add_subdirectory_ifdef(CONFIG_BLE_MESH ${CMAKE_CURRENT_LIST_DIR}/examples/bluetooth-le/mesh/${CONFIG_BUILD_PORJECT})
