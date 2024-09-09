
# Component Makefile
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += Inc
## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src 
COMPONENT_SRCS := Src/efd_iap.c \
				  Src/efd_kv.c \
				  Src/efd_log.c \
				  Src/efd_port.c \
				  Src/efd_utils.c \
				  Src/EnhancedFlashDataset.c 

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
COMPONENT_SRCDIRS := Src

