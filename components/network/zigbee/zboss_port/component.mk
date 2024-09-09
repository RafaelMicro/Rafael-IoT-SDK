
# Component Makefile
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += include 
## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := 

## This component's src 
COMPONENT_SRCS := src/zb_radio.c \
				  src/zb_freertos.c \
				  src/zb_crypto.c \
				  src/zb_nvram.c \
				  src/zb_timer.c \
				  src/zb_serial.c

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
COMPONENT_SRCDIRS := src
