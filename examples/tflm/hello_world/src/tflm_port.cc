/*
 * Copyright (c) 2026 Rafael Microelectronics Inc. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 * Minimal C++ runtime shims for building TFLM (C++) on bare-metal newlib-nano.
 * The executable is linked with the C driver (see the example CMakeLists), which
 * pulls in the newlib C runtime (crt*, providing __dso_handle and __cxa_atexit)
 * but NOT libstdc++/libsupc++. TFLM builds -fno-exceptions -fno-rtti and, with
 * TF_LITE_STATIC_MEMORY, does not allocate from the heap at runtime - but the
 * compiler still emits references to operator new/delete and __cxa_pure_virtual,
 * normally provided by libsupc++. These definitions satisfy the linker; the
 * operators defer to newlib malloc/free and are not expected to be exercised by
 * the static-memory TFLM runtime.
 */

#include <cstddef>
#include <cstdlib>

void* operator new(size_t size) { return malloc(size); }
void* operator new[](size_t size) { return malloc(size); }
void operator delete(void* ptr) noexcept { free(ptr); }
void operator delete[](void* ptr) noexcept { free(ptr); }
void operator delete(void* ptr, size_t) noexcept { free(ptr); }
void operator delete[](void* ptr, size_t) noexcept { free(ptr); }

extern "C" {

// Called if a pure-virtual method is ever invoked; must not return. (Provided by
// libsupc++ normally, which is not linked here.)
void __cxa_pure_virtual() {
  for (;;) {
  }
}

}  // extern "C"
