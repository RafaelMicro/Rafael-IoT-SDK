
# Component Makefile
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += include
## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src 
COMPONENT_SRCS := Src/comm_subsystem_drv.c \
		  Src/rf_common_init.c \
		  Src/rf_mcu.c \
		  Src/rf_mcu_ahb.c \
		  Src/rf_mcu_spi.c 
		  
COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
COMPONENT_SRCDIRS := Src
