# Definitions common to all certification tests

TEST_STEP_CONTROL_SRCS = $(BUILD_HOME)/tests/multitest_common/zb_test_step_control.c \
						 $(BUILD_HOME)/tests/multitest_common/zb_test_step_storage.c

COMMON_SRCS_ZRZC = ../common/zb_cert_test_globals.c $(TEST_STEP_CONTROL_SRCS)
COMMON_SRCS_ZED  = ../common/zb_cert_test_globals.c $(TEST_STEP_CONTROL_SRCS)
