
# Component Makefile
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += Inc \
			     CMSIS/Include

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := 

## This component's src 
COMPONENT_SRCS := Src/system_mcu.c \
		  Src/comparator.c \
		  Src/dma.c \
		  Src/flashctl.c \
		  Src/gpio.c \
		  Src/i2c_master.c \
		  Src/i2s.c \
		  Src/lpm.c \
		  Src/mp_sector.c \
		  Src/pwm.c \
		  Src/qspi.c \
		  Src/rtc.c \
		  Src/sadc.c \
		  Src/swi.c \
		  Src/swt.c \
		  Src/sysctrl.c \
		  Src/sysfun.c \
		  Src/timer.c \
		  Src/wdt.c

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
COMPONENT_SRCDIRS := Src
