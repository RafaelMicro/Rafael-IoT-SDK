toolchains := gcc

# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += 
							 
## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=							 

## This component's src 
COMPONENT_SRCS1 := bootloader/src/boot/$(toolchains)/startup.S \
				  bootloader/src/system_cm3_mcu.c \
				  bootloader/src/sscanf.c \
				  bootloader/src/vsscanf.c \
				  bootloader/src/strntoumax.c
COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS1))
COMPONENT_OBJS := $(patsubst %.S,%.o, $(COMPONENT_OBJS))
COMPONENT_SRCS := $(COMPONENT_SRCS1)
COMPONENT_SRCDIRS := bootloader/src/boot/$(toolchains) bootloader/src

ifneq ($(CONFIG_LINK_CUSTOMER),1)

ifeq ($(CONFIG_GEN_ROM),1)
	LINKER_SCRIPTS := rom.ld
else
	ifeq ($(CONFIG_LINK_RAM),1)
		ifeq ($(CONFIG_BUILD_ROM_CODE),1)
			LINKER_SCRIPTS := ram.ld
		else
			LINKER_SCRIPTS := ram_rom.ld
		endif
	else
		ifeq ($(CONFIG_BUILD_ROM_CODE),1)
			ifeq ($(CONFIG_USE_PSRAM),1)
				LINKER_SCRIPTS := psram_flash.ld
			else
				LINKER_SCRIPTS := flash.ld
			endif
		else
			LINKER_SCRIPTS := flash_rom.ld
		endif
	endif
endif

LINKER_SCRIPTS := rt582.ld

##
COMPONENT_ADD_LDFLAGS += -L $(COMPONENT_PATH)/bootloader/ld \
                         $(addprefix -T ,$(LINKER_SCRIPTS))
##                        
COMPONENT_ADD_LINKER_DEPS := $(addprefix bootloader/ld/,$(LINKER_SCRIPTS))

endif

##
#CPPFLAGS += -D
