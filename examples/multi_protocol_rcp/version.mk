PROJ_VER := 3.6.2-
GIT_HASH := $(shell cd ${RAFAEL_IOT_SDK_PATH} && git describe --always --dirty)
EXTRA_CPPFLAGS ?=
EXTRA_CPPFLAGS += -D RAFAEL_SDK_VER=\"$(PROJ_VER)$(GIT_HASH)\"
