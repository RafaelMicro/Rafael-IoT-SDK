if((${CONFIG_CHIP} STREQUAL "RT582"))
    sdk_add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/rt582)

elseif((${CONFIG_CHIP} STREQUAL "RT584"))
    sdk_add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/rt584)
endif()

