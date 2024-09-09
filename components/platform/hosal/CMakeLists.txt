if((${CONFIG_CHIP} STREQUAL "RT582"))
    sdk_add_subdirectory_ifdef(CONFIG_HOSAL ${CMAKE_CURRENT_LIST_DIR}/rt582_hosal)
elseif((${CONFIG_CHIP} STREQUAL "RT584"))
    sdk_add_subdirectory_ifdef(CONFIG_HOSAL ${CMAKE_CURRENT_LIST_DIR}/rt584_hosal)
endif()

