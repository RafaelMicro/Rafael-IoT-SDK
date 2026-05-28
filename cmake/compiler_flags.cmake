sdk_add_link_options(
-Wl,--gc-sections
-Wl,-static
-Wl,--start-group
-Wl,--end-group
-Wl,-EL
-Wl,-t
-Wl,--cref
-Wl,--print-memory-usage
)

#sdk_add_link_libraries(c m)
if(NOT CONFIG_STRIP_DEBUG)
    sdk_add_compile_options(-g -gdwarf-2)
else()
    # Release: prevent GCC from embedding its version string in .comment sections
    sdk_add_compile_options(-fno-ident)
    sdk_add_compile_options(
        -fmacro-prefix-map=${CMAKE_SOURCE_DIR}/=./
        -ffile-prefix-map=${CMAKE_SOURCE_DIR}/=./
    )
endif()

sdk_add_compile_options(
    -specs=nano.specs
    -lc -lm -lnosys
    -ffunction-sections -fdata-sections
    -fstrict-volatile-bitfields
    -fcommon -ffreestanding
    -fno-strict-aliasing
    -fno-zero-initialized-in-bss
    -Wno-error=unused-function -Wno-error=unused-but-set-variable
    -Wno-error=unused-variable -Wno-error=deprecated-declarations
    -Wextra -Wno-unused-parameter
    -Wno-sign-compare -Wno-address-of-packed-member
    -DUSE_FREERTOS
)
