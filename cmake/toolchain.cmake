# ARM
set(CMAKE_SYSTEM_NAME              Generic)
set(CMAKE_SYSTEM_PROCESSOR         ARM)
set(CROSS_COMPILE arm-none-eabi-)

# specify cross compilers and tools
SET(CMAKE_C_COMPILER ${TOOLCHAIN_BIN_DIR}${CROSS_COMPILE}gcc${TOOLCHAIN_SUFFIX} )
message("C Compiler: ${CMAKE_C_COMPILER}")
SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_BIN_DIR}${CROSS_COMPILE}g++${TOOLCHAIN_SUFFIX} )
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_BIN_DIR}${CROSS_COMPILE}gcc${TOOLCHAIN_SUFFIX} )
set(CMAKE_LINKER ${TOOLCHAIN_BIN_DIR}${CROSS_COMPILE}ld${TOOLCHAIN_SUFFIX} )
set(CMAKE_AR ${TOOLCHAIN_BIN_DIR}${CROSS_COMPILE}ar${TOOLCHAIN_SUFFIX} )
set(CMAKE_RANLIB ${TOOLCHAIN_BIN_DIR}${CROSS_COMPILE}ranlib${TOOLCHAIN_SUFFIX} )
set(CMAKE_OBJCOPY ${TOOLCHAIN_BIN_DIR}${CROSS_COMPILE}objcopy )
set(CMAKE_OBJDUMP ${TOOLCHAIN_BIN_DIR}${CROSS_COMPILE}objdump )
set(CMAKE_SIZE ${TOOLCHAIN_BIN_DIR}${CROSS_COMPILE}size )

set(CPU_COMPILER_SECURE_STR "")
if((DEFINED CONFIG_RF1301) OR (DEFINED CONFIG_RT584H) OR (DEFINED CONFIG_RT584HA4) OR (DEFINED CONFIG_RT584L)
OR (DEFINED CONFIG_RT584H_NONE_OS) OR (DEFINED CONFIG_RT584L_NONE_OS) OR (DEFINED CONFIG_RF1301_NONE_OS) OR (DEFINED CONFIG_RT584HA4_NONE_OS))
if((${CONFIG_TRUST_ZONE_SECURE}))
    set(CPU_COMPILER_SECURE_STR "-mcmse")
endif()
endif()

# Float ABI is derived from the CPU (CONFIG_CPU_COMPILER_FLAG is present in every
# config), so no per-config symbol is needed. Cortex-M33 has an FPU -> softfp
# (hardware FPU instructions with the soft-float calling convention) so the image
# stays ABI-compatible with the prebuilt soft-float libraries shipped in this SDK
# (libCM33_ble_host.a, zboss, ruci, ...); -mfloat-abi=hard would break linkage.
# Cortex-M3 has no FPU -> soft.
if(CONFIG_CPU_COMPILER_FLAG MATCHES "cortex-m33")
    set(CPU_FLOAT_FLAG "-mfloat-abi=softfp -mfpu=fpv5-sp-d16")
else()
    set(CPU_FLOAT_FLAG "-mfloat-abi=soft")
endif()

# -fomit-frame-pointer is required, not cosmetic: mbedtls builds its bignum at -O0
# (CMAKE_C_FLAGS_DEBUG in its own CMakeLists), and at -O0 GCC pins the frame pointer
# to r7. The MULADDC inline asm in bn_mul.h (enabled on M33 via __ARM_FEATURE_DSP)
# needs 8 registers with its carry operand constrained to a low register (r0-r7).
# With the FPU enabled that no longer fits and GCC 14.2 fails with "'asm' operand
# has impossible constraints or there are not enough registers". Freeing r7 fixes it
# -- this is mbedtls' own documented workaround for -O0 on ARM (see its ChangeLog).
set(COMMON_C_FLAGS                 "${CONFIG_CPU_COMPILER_FLAG} ${CPU_COMPILER_SECURE_STR} ${CPU_FLOAT_FLAG} -mthumb -mabi=aapcs -fomit-frame-pointer -fdata-sections -ffunction-sections -Wno-format")
set(CMAKE_C_FLAGS_INIT             "${COMMON_C_FLAGS} -std=gnu99")
# -nodefaultlibs, NOT -nostdlib: a target that links a C++ static library (openthread)
# gets CXX as its linker language, so these flags drive the link. -nostdlib drops the
# standard libraries *and the startup files*, and without crt0 there is no _start --
# which CMSIS 6's __cmsis_start (the default __PROGRAM_START) calls. -nodefaultlibs
# drops only the libraries and keeps crt0, so C++ links behave like the C ones
# (_start -> __libc_init_array -> main); the needed libs are passed explicitly below.
set(CMAKE_CXX_FLAGS_INIT           "${COMMON_C_FLAGS} -std=c++11 -nodefaultlibs -fno-rtti -fno-exceptions")
set(CMAKE_ASM_FLAGS_INIT           "${COMMON_C_FLAGS} -nostartfiles -nostdlib -nodefaultlibs -ffreestanding -lnosys")
set(CMAKE_EXE_LINKER_FLAGS_INIT    "${COMMON_C_FLAGS} -specs=nano.specs -specs=nosys.specs")

set(CMAKE_C_FLAGS_DEBUG            "-Os -g")
set(CMAKE_CXX_FLAGS_DEBUG          "-Os -g")
set(CMAKE_ASM_FLAGS_DEBUG          "-g")

set(CMAKE_C_FLAGS_RELEASE          "-Os")
set(CMAKE_CXX_FLAGS_RELEASE        "-Os")
set(CMAKE_ASM_FLAGS_RELEASE        "")

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

set(CMAKE_FIND_ROOT_PATH ${CROSS_COMPILE}gcc)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
