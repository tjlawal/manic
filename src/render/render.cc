#include "render/core/render_core.cc"

#if OS_WINDOWS
	#if RENDER_SW
		#include "render/software/win32/render_sw_win32.cc"
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
	#if RENDER_VULKAN
	#elif RENDER_OPENGL
	#else 
		#error "Rendering backend not specified."
	#endif
#else
	#error "This operating system is not supported."
#endif

namespace Starlight {
	namespace Render {
	}
}