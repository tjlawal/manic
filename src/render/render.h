#pragma once

#include "render/core/render_core.h"
#if OS_WINDOWS
	#if RENDER_SW
		#include "render/software/win32/render_sw_win32.h"
	#elif RENDER_D3D11
	#elif RENDER_D3D12
	#elif RENDER_VULKAN
	#else
		#error "Rendering backend not specified."
	#endif
#elif OS_MACOS
	// Metal is the only choice on macs!
	#if RENDER_SW
	#elif RENDER_METAL
	#else
		#error "Rendering backend not specified."
	#endif
#elif OS_LINUX
	#if RENDER_SW
	#elif RENDER_VULKAN
	#elif RENDER_OPENGL
	#else 
		#error "Rendering backend not specified."
	#endif
#else
	#error "This operating system is not supported."
#endif

// Zero out things

// Graphics Renderer
#if !defined(RENDER_SW)
	#define RENDER_SW 0
#endif

#if !defined(RENDER_D3D11)
	#define RENDER_D3D11 0
#endif

#if !defined(RENDER_D3D12)
	#define RENDER_D3D12 0
#endif

#if !defined(RENDER_VULKAN)
	#define RENDER_VULKAN 0
#endif

#if !defined(RENDER_METAL)
	#define RENDER_METAL 0
#endif

#if !defined(RENDER_OPENGL)
	#define RENDER_OPENGL 0
#endif