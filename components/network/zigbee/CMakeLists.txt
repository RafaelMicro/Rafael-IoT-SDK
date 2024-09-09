message("\t+ Zigbee")


sdk_add_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/zboss_port/include
    ${CMAKE_CURRENT_LIST_DIR}/zboss/include
    ${CMAKE_CURRENT_LIST_DIR}/zboss/platform/include
    ${CMAKE_CURRENT_LIST_DIR}/zboss/platform/osif/include
)

add_compile_options()
add_definitions()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/zboss_port)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/zboss)

# if(${${CONFIG_THREAD_DISABLE_PRINT}})
#     add_definitions(-DOPENTHREAD_CONFIG_LOG_OUTPUT=OPENTHREAD_CONFIG_LOG_OUTPUT_NONE)
# else()
#     add_definitions(-DOPENTHREAD_CONFIG_LOG_LEVEL=OT_LOG_LEVEL_DEBG)
#     add_definitions(-DOPENTHREAD_CONFIG_LOG_LEVEL_INIT=OT_LOG_LEVEL_NONE)
#     add_definitions(-DOPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE=1)
# endif()
