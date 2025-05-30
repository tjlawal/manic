
#include "platform/core/platform_core.cc"
#include "platform/gfx/platform_gfx.cc"

#if OS_WINDOWS 
	#include "platform/core/win32/platform_win32.cc"
	#include "platform/gfx/win32/platform_gfx_win32.cc"
#else
	#error "This platform is not supported."
#endif
