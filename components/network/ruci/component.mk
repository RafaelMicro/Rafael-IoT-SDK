
# Component Makefile
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += Inc
## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src 
COMPONENT_SRCS := Src/ruci.c \
		  Src/ruci_cmn_event.c \
		  Src/ruci_cmn_hal_cmd.c \
		  Src/ruci_cmn_sys_cmd.c \
		  Src/ruci_pci15p4_mac_cmd.c \
		  Src/ruci_pci_ble_cmd.c \
		  Src/ruci_pci_common_cmd.c \
		  Src/ruci_pci_data.c \
		  Src/ruci_pci_event.c \
		  Src/ruci_pci_fsk_cmd.c \
		  Src/ruci_pci_oqpsk_cmd.c \
		  Src/ruci_pci_slink_cmd.c \
		  Src/ruci_pci_ook_cmd.c \
		  Src/ruci_pci_zwave_cmd.c \
		  Src/ruci_sf_event.c \
		  Src/ruci_sf_host_cmd.c \
		  Src/ruci_sf_host_event.c \
		  Src/ruci_sf_host_sys_cmd.c
		  
COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
COMPONENT_SRCDIRS := Src
