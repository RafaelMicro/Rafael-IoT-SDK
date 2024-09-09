
# Component Makefile
#
COMPONENT_ADD_INCLUDEDIRS += config Inc portable/GCC/ARM_CM3

COMPONENT_OBJS := $(patsubst %.c,%.o, \
                    event_groups.c \
                    list.c \
                    queue.c \
                    stream_buffer.c \
                    tasks.c \
                    timers.c \
                    portable/GCC/ARM_CM3/port.c \
                    portable/MemMang/heap_5.c)

COMPONENT_OBJS := $(patsubst %.S,%.o, $(COMPONENT_OBJS))

COMPONENT_SRCDIRS := . portable portable/GCC/ARM_CM3 portable/MemMang

OPT_FLAG_G := $(findstring -Og, $(CFLAGS))
ifeq ($(strip $(OPT_FLAG_G)),-Og)
CFLAGS := $(patsubst -Og,-O2,$(CFLAGS))
endif
OPT_FLAG_S := $(findstring -Os, $(CFLAGS))
ifeq ($(strip $(OPT_FLAG_S)),-Os)
CFLAGS := $(patsubst -Os,-O2,$(CFLAGS))
endif


