#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

ifdef CONFIG_OPERATION_UART_PORT
CPPFLAGS += -DCONFIG_OPERATION_UART_PORT=$(CONFIG_OPERATION_UART_PORT)
endif


COMPONENT_ADD_INCLUDEDIRS += Include
