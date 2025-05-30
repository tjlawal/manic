#ifndef PLATFORM_INC_H
#define PLATFORM_INC_H

#include "platform/core/platform_core.h"
#include "platform/gfx/platform_gfx.h"

#if OS_WINDOWS 
	#include "platform/core/win32/platform_win32.h"
	#include "platform/gfx/win32/platform_gfx_win32.h"
#else
	#error "This platform is not supported."
#endif

#endif // PLATFORM_INC_H