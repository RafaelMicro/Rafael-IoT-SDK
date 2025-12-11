if(${CONFIG_CHIP} STREQUAL "RT581"
   OR ${CONFIG_CHIP} STREQUAL "RT582"
   OR ${CONFIG_CHIP} STREQUAL "RT583")
   message(STATUS "Using RT582 configuration")
  sdk_add_include_directories(config/rt582)
else()
  sdk_add_include_directories(config/rt584)
endif()