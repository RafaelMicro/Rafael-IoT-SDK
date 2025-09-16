message("\t+ Zigbee")


sdk_add_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/zboss_port/include
    ${CMAKE_CURRENT_LIST_DIR}/zboss/include
)

add_compile_options()
add_definitions()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/zboss_port)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/zboss)
