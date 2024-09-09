
ifdef CONFIG_HOSAL_RF_DEBUG
CPPFLAGS += -DCONFIG_HOSAL_RF_DEBUG=$(CONFIG_HOSAL_RF_DEBUG)
endif

# Component Makefile
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += Inc
## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src 
COMPONENT_SRCS := Src/hosal_uart.c \
				  Src/hosal_rf.c

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
COMPONENT_SRCDIRS := Src

