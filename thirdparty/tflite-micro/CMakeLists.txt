# TensorFlow Lite for Microcontrollers (TFLM).
#
# Self-contained upstream C++ inference runtime, vendored under thirdparty/ like
# FreeRTOS. Built into a static library `tflm` and whole-archive linked by
# setup_project() (registered in SDK_LIBS via sdk_generate_library). Opt-in via
# CONFIG_TFLM; wired from the top-level CMakeLists.txt with
# sdk_add_subdirectory_ifdef(CONFIG_TFLM ...). See VERSION.txt for provenance.
#
# The int8/int16 kernels use CMSIS-NN as the optimized backend. CONFIG_TFLM
# selects CONFIG_CMSIS_NN (Kconfig), and the vendored CMSIS-NN v7.0.0 at
# components/platform/common/CMSIS-NN is reused (no second copy) - its parent dir
# is added to the include path there so the "Include/arm_nnfunctions.h" includes
# in kernels/cmsis_nn/*.cc resolve.

sdk_generate_library(tflm)

# Tree root on the include path so "tensorflow/lite/micro/..." resolves, plus the
# header-only third-party deps at the roots their #includes expect:
#   flatbuffers -> "flatbuffers/flatbuffers.h"   gemmlowp -> "fixedpoint/..."
#   ruy         -> "ruy/profiler/..."
sdk_add_include_directories(.)
sdk_add_include_directories(third_party/flatbuffers/include)
sdk_add_include_directories(third_party/gemmlowp)
sdk_add_include_directories(third_party/ruy)
# kissfft root: the signal kiss_fft wrappers #include "kiss_fft.c"/"tools/..."
# (unity-style) directly, so its directory must be on the include path.
sdk_add_include_directories(third_party/kissfft)

# One function/op per .cc; glob the vendored tree (core runtime + kernels under
# tensorflow/, plus the signal library used by micro_ops.h). CONFIGURE_DEPENDS
# re-globs when the file set changes (e.g. a version bump). Test/benchmark files
# were stripped at vendoring time; the filter is defensive against future imports.
# kissfft's kiss_fft.c/kiss_fftr.c are intentionally NOT globbed: the signal
# kiss_fft wrappers #include them directly (unity build).
file(GLOB_RECURSE TFLM_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/tensorflow/*.cc
    ${CMAKE_CURRENT_LIST_DIR}/tensorflow/*.c
    ${CMAKE_CURRENT_LIST_DIR}/signal/*.cc
    ${CMAKE_CURRENT_LIST_DIR}/signal/*.c)
# Exclude only genuine unit tests / benchmarks. NOTE: test_helpers.cc,
# mock_micro_graph.cc, kernel_runner.cc and test_helper_custom_ops.cc are library
# support files (create_tflm_tree lists them as library sources and they
# reference each other), so they are compiled - do not exclude them here.
list(FILTER TFLM_SOURCES EXCLUDE REGEX "(_test|_benchmark)\\.(cc|c)$")
sdk_library_add_sources(${TFLM_SOURCES})

# TFLM needs C++17 and builds without exceptions/RTTI/threadsafe-static guards.
# The global C++ standard is c++11 (cmake/toolchain.cmake); the later -std wins.
# (The SDK-global -ffreestanding is scoped to non-C++ in compiler_flags.cmake so
# TFLM's hosted-libstdc++ headers - <functional>, <vector>, <cmath> - compile,
# matching how upstream TFLM builds.)
target_compile_options(tflm PRIVATE -std=c++17 -fno-threadsafe-statics)

# Global (interface) so both this library AND the app that includes TFLM headers
# compile them the same way:
#   TF_LITE_STATIC_MEMORY - no dynamic allocation (arena-based)
#   CMSIS_NN              - select the cmsis_nn kernel path in the kernel headers
#   TF_LITE_MCU_DEBUG_LOG - use the MCU abort path (MicroPrintf + halt) in
#                           op_macros.h instead of libc abort()
sdk_add_compile_definitions(TF_LITE_STATIC_MEMORY CMSIS_NN TF_LITE_MCU_DEBUG_LOG)
