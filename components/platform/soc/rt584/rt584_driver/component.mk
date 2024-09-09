
# Component Makefile
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += Inc \
			     CMSIS/Include

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := 

## This component's src 
COMPONENT_SRCS := \
		  Src/dma.c \
		  Src/flashctl.c \
		  Src/gpio.c \
		  Src/otp.c \
		  Src/pwm.c \
		  Src/qspi.c \
		  Src/rtc.c \
		  Src/sadc.c \
		  Src/swi.c \
		  Src/sysctrl.c \
		  Src/sysfun.c \
		  Src/system_cm33.c \
		  Src/timer.c \
		  Src/trng.c \
		  Src/wdt.c

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
COMPONENT_SRCDIRS := Src
