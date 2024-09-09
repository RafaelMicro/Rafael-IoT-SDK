ifdef CONFIG_UART_STDIO_PORT
CPPFLAGS += -DCONFIG_UART_STDIO_PORT=$(CONFIG_UART_STDIO_PORT)
endif
ifdef CONFIG_UART_STDIO_BAUDRATE
CPPFLAGS += -DCONFIG_UART_STDIO_BAUDRATE=$(CONFIG_UART_STDIO_BAUDRATE)
endif

# Component Makefile
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += Inc
## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src 
COMPONENT_SRCS := uart_stdio.c

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
COMPONENT_SRCDIRS := .

