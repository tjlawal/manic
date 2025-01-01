// Copyright Frost Gorilla, Inc. All Rights Reserved.

#include "os/core/os_core.c"

#if OS_FEATURE_GRAPHICAL
  #include "os/gfx/os_gfx.c"
#endif

#if OS_WINDOWS
  #include "os/core/win32/os_core_win32.c"
#elif OS_LINUX
  #include "os/core/linux/os_core_linux.c"
#elif OS_MAC
  // NOTE(tijani): don't know if this would work even.
  // will correct as needed at that stage.
  #include "os/core/mac/os_core_mac.c"
#else
  #error OS layer is not implemented for this operating system.
#endif

#if OS_FEATURE_GRAPHICAL
  #if OS_GFX_STUB
    #include "gfx/stub/os_gfx_stub.c"
  #elif OS_WINDOWS
    #include "gfx/win32/os_gfx_win32.c"
  #elif OS_LINUX
    #include "gfx/linux/os_gfx_linux.c"
  #elif OS_MAC
    #include "gfx/mac/os_gfx_mac.c"
  #else
    #error OS graphical layer not implemented for this operating system.
  #endif

#endif
