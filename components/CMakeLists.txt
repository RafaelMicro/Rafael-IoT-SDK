message(STATUS "Apply Component List:")

sdk_add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/platform)
sdk_add_subdirectory_ifdef(CONFIG_BUILD_COMPONENT_NEWLIBC ${CMAKE_CURRENT_LIST_DIR}/libc)
sdk_add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/utility)
sdk_add_subdirectory_ifdef(CONFIG_BUILD_COMPONENT_CPC ${CMAKE_CURRENT_LIST_DIR}/cpc)
sdk_add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/network)
sdk_add_subdirectory_ifdef(CONFIG_FREERTOS
                           ${CMAKE_CURRENT_LIST_DIR}/rtos_port)