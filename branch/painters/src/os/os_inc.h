// Copyright Frost Gorilla, Inc. All Rights Reserved.

#ifndef OS_INC_H
#define OS_INC_H

#if !defined(OS_FEATURE_GRAPHICAL)
  #define OS_FEATURE_GRAPHICAL 0
#endif

#if !defined(OS_GFX_STUB)
  #define OS_GFX_STUB 0
#endif

#include "core/os_core.h"

#if OS_FEATURE_GRAPHICAL
  #include "gfx/os_gfx.h"
#endif

#if OS_WINDOWS
  #include "os/core/win32/os_core_win32.h"
#elif OS_LINUX
  #include "os/core/linux/os_core_linux.h"
#elif OS_MAC
  #include "os/core/mac/os_core_mac.h"
#else
  #error OS layer is not implemented for this operating system.
#endif

#if OS_FEATURE_GRAPHICAL
  #if OS_GFX_STUB
    #include "gfx/stub/os_gfx_stub.h"
  #elif OS_WINDOWS
    #include "gfx/win32/os_gfx_win32.h"
  #elif OS_LINUX
    #include "gfx/linux/os_gfx_linux.h"
  #elif OS_MAC
    #include "gfx/mac/os_gfx_mac.h"
  #else
    #error OS graphical layer not implemented for this operating system.
  #endif

#endif

#endif // OS_INC_H
