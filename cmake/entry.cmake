if(MINGW OR CYGWIN OR WIN32)
    set(CMAKE_SYSTEM_NAME Generic)
    set(WHERE_CMD where)
    set(TOOLCHAIN_SUFFIX ".exe")
elseif(UNIX)
    set(CMAKE_SYSTEM_NAME Linux)
    set(WHERE_CMD which)
    set(TOOLCHAIN_SUFFIX "")
elseif(APPLE)
    set(CMAKE_SYSTEM_NAME Darwin)
    set(WHERE_CMD which)
    set(TOOLCHAIN_SUFFIX "")
endif()

SET(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR ARM)
set(PROJECT_BINARY_DIR ${CMAKE_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

add_library(sdk_intf_lib INTERFACE)

# Include paths only. Split out of sdk_intf_lib so a library can opt out of
# the global include pool (sdk_library_strict_includes()) while still
# getting every CONFIG_* compile definition and compile/link option, which
# stay on sdk_intf_lib. See docs/internal/adr/0001-additive-only-sdk-dsl.md.
add_library(sdk_intf_includes INTERFACE)

include(${CMAKE_CURRENT_LIST_DIR}/extension.cmake)
sdk_ifndef(CUSTOM_CONFIG_DIR ".config")
config_parse("${CMAKE_SOURCE_DIR}/${CUSTOM_CONFIG_DIR}")

include(${CMAKE_CURRENT_LIST_DIR}/toolchain.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/compiler_flags.cmake)

if(NOT CONFIG_STRIP_DEBUG)
    sdk_add_compile_options(${CONFIG_COMPILE_OPTIMIZATION} -g3)
else()
    sdk_add_compile_options(${CONFIG_COMPILE_OPTIMIZATION})
endif()