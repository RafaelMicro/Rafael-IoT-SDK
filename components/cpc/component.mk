# Component Makefile
#
ifdef CONFIG_CPC_RX_PAYLOAD_MAX_LENGTH
CPPFLAGS += -DCONFIG_CPC_RX_PAYLOAD_MAX_LENGTH=$(CONFIG_CPC_RX_PAYLOAD_MAX_LENGTH)
endif

ifdef CONFIG_CPC_CRC_0
CPPFLAGS += -DCONFIG_CPC_CRC_0=$(CONFIG_CPC_CRC_0)
endif

## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += inc
							 
## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := config

## This component's src 
COMPONENT_SRCS := src/cpc_crc.c \
                  src/cpc_dispatcher.c \
                  src/cpc_drv_secondary_uart_usart.c \
                  src/cpc_hdlc.c \
				  src/cpc_system_secondary.c \
				  src/cpc.c \
				  src/cpc_timer.c \
				  src/slist.c \
				  src/mem_pool.c \
				  src/cpc_memory.c \

				  
COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := src

##
#CPPFLAGS += 
