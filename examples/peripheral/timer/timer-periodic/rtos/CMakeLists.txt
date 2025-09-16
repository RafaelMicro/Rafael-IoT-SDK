if(${CONFIG_CHIP} STREQUAL "RT581"
   OR ${CONFIG_CHIP} STREQUAL "RT582"
   OR ${CONFIG_CHIP} STREQUAL "RT583")
  sdk_add_include_directories(config/rt582)
else()
  sdk_add_include_directories(config/rt584)
endif()


