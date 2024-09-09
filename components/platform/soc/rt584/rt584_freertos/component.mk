
# Component Makefile
#
COMPONENT_ADD_INCLUDEDIRS += config Inc portable/GCC/ARM_CM33/secure portable/GCC/ARM_CM33_NTZ/non_secure

COMPONENT_OBJS := $(patsubst %.c,%.o, \
                    event_groups.c \
                    list.c \
                    queue.c \
                    stream_buffer.c \
                    tasks.c \
                    timers.c \
                    portable/GCC/ARM_CM33/secure/secure_context.c \
                    portable/GCC/ARM_CM33/secure/secure_context_port.c \
                    portable/GCC/ARM_CM33/secure/secure_heap.c \
                    portable/GCC/ARM_CM33/secure/secure_init.c \
                    portable/GCC/ARM_CM33_NTZ/non_secure/port.c \
                    portable/GCC/ARM_CM33_NTZ/non_secure/portasm.c)

COMPONENT_OBJS := $(patsubst %.S,%.o, $(COMPONENT_OBJS))

COMPONENT_SRCDIRS := . portable portable/GCC/ARM_CM33/secure portable/GCC/ARM_CM33_NTZ/non_secure portable/MemMang

OPT_FLAG_G := $(findstring -Og, $(CFLAGS))
ifeq ($(strip $(OPT_FLAG_G)),-Og)
CFLAGS := $(patsubst -Og,-O2,$(CFLAGS))
endif
OPT_FLAG_S := $(findstring -Os, $(CFLAGS))
ifeq ($(strip $(OPT_FLAG_S)),-Os)
CFLAGS := $(patsubst -Os,-O2,$(CFLAGS))
endif


