if(${CONFIG_CHIP} STREQUAL "RT581"
   OR ${CONFIG_CHIP} STREQUAL "RT582"
   OR ${CONFIG_CHIP} STREQUAL "RT583")
   message(STATUS "Using RT582 configuration")
  sdk_set_freertos_config_directory(config/rt582)
else()
  sdk_set_freertos_config_directory(config/rt584)
endif()