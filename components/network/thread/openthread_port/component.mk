# Component Makefile
#

include $(COMPONENT_PATH)/../openthread_common.mk

## These include paths would be exported to project level
ifeq ("$(CONFIG_TARGET_CUSTOMER)", "das")
COMPONENT_ADD_INCLUDEDIRS += ../subg_openthread/include ../subg_openthread/src/core ../subg_openthread/src ../subg_openthread/examples/platforms
else
COMPONENT_ADD_INCLUDEDIRS += ../openthread/include ../openthread/src/core ../openthread/src ../openthread/examples/platforms
endif

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src 
COMPONENT_SRCS := src/ot_alarm.c \
                  src/ot_diag.c \
                  src/ot_entropy.c \
                  src/ot_settings.c \
                  src/ot_logging.c \
                  src/ot_misc.c \
                  src/ot_radio.c \
                  src/ot_uart.c \
                  src/ot_freertos.c \
                  src/ot_memory.c \
                  utils/mac_frame_gen.c \
                  utils/soft_source_match_table.c \

ifdef CFG_CPC_ENABLE
COMPONENT_SRCS += ncp/ncp.c 
COMPONENT_SRCS += ncp/ncp_cpc.cpp

CPPFLAGS += -DCONFIG_OT_RCP_EZMESH
endif
COMPONENT_OBJS := $(patsubst %.cpp,%.o, $(filter %.cpp,$(COMPONENT_SRCS))) $(patsubst %.c,%.o, $(filter %.c,$(COMPONENT_SRCS))) $(patsubst %.S,%.o, $(filter %.S,$(COMPONENT_SRCS)))
COMPONENT_SRCDIRS := src utils ncp