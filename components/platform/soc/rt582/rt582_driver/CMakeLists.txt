sdk_generate_library(rt582_driver)
sdk_add_include_directories(
    CMSIS/Include
    Inc
)
if(CONFIG_BOOTLOADER)
sdk_library_add_sources(
    Src/system_mcu.c
    Src/dma.c
    Src/gpio.c
    Src/flashctl.c
    Src/sysctrl.c
    Src/timer.c
    # Src/sysfun.c 
)
else()
sdk_library_add_sources(
    Src/system_mcu.c
    Src/comparator.c
    # Src/uart_drv.c
    Src/dma.c
    Src/flashctl.c
    Src/gpio.c
    Src/i2c_master.c
    Src/i2s.c
    Src/lpm.c
    Src/mp_sector.c
    Src/pwm.c
    # Src/qspi.c
    Src/rtc.c
    Src/sadc.c
    Src/swi.c
    Src/sysctrl.c
    Src/sysfun.c
    Src/timer.c
    Src/wdt.c
)
endif()
