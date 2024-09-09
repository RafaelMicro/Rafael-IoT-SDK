# ARM
set(CMAKE_SYSTEM_NAME              Generic)
set(CMAKE_SYSTEM_PROCESSOR         ARM)
set(CROSS_COMPILE arm-none-eabi-)

# specify cross compilers and tools
SET(CMAKE_C_COMPILER ${CROSS_COMPILE}gcc${TOOLCHAIN_SUFFIX} )
SET(CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++${TOOLCHAIN_SUFFIX} )
set(CMAKE_ASM_COMPILER ${CROSS_COMPILE}gcc${TOOLCHAIN_SUFFIX} )
set(CMAKE_LINKER ${CROSS_COMPILE}ld${TOOLCHAIN_SUFFIX} )
set(CMAKE_AR ${CROSS_COMPILE}ar${TOOLCHAIN_SUFFIX} )
set(CMAKE_RANLIB ${CROSS_COMPILE}ranlib${TOOLCHAIN_SUFFIX} )
set(CMAKE_OBJCOPY ${CROSS_COMPILE}objcopy )
set(CMAKE_OBJDUMP ${CROSS_COMPILE}objdump )
set(CMAKE_SIZE ${CROSS_COMPILE}size )

set(CPU_COMPILER_SECURE_STR "")
if((${CONFIG_RT584}))
if((${CONFIG_TRUST_ZONE} STREQUAL "TRUST_ZONE_SECURE"))
    set(CPU_COMPILER_SECURE_STR "-mcmse")
endif()
endif()

set(COMMON_C_FLAGS                 "${CONFIG_CPU_COMPILER_FLAG} ${CPU_COMPILER_SECURE_STR} -mfloat-abi=soft -mfpu=fpv4-sp-d16 -mthumb -mabi=aapcs -fdata-sections -ffunction-sections -Wno-format")
set(CMAKE_C_FLAGS_INIT             "${COMMON_C_FLAGS} -std=gnu99")
set(CMAKE_CXX_FLAGS_INIT           "${COMMON_C_FLAGS} -std=c++11 -nostdlib -fno-rtti -fno-exceptions")
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
