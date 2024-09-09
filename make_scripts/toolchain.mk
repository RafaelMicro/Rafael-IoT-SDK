##change to your toolchain path
CONFIG_TOOLPREFIX ?= $(RAFAEL_IOT_SDK_PATH)/toolchain/arm/$(shell uname |cut -d '_' -f1)/bin/arm-none-eabi-
#CONFIG_TOOLPREFIX ?= arm-none-eabi-
