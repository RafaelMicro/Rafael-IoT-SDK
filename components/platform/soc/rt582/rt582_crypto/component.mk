
# Component Makefile
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += Inc
## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src 
COMPONENT_SRCS := rt_aes.c \
				  rt_crypto_util.c \
				  rt_crypto.c \
				  rt_ecjpake.c \
				  rt_hmac_drbg.c \
				  rt_sha256.c \

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
COMPONENT_SRCDIRS := .

