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
sdk_add_compile_options(
    -g -gdwarf-2
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
    -DUSE_FREERTOS -save-temps=obj
    -fmacro-prefix-map=old=new -ffile-prefix-map=old=new
)
