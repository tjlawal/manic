#pragma once

using namespace Starlight::Foundation;
using namespace Starlight::Platform;

// Render Modes
#define RENDERMODE_DEFAULT           (1 << 0)
#define RENDERMODE_WIREFRAME         (1 << 1)
#define RENDERMODE_VERTEXPOINTS      (1 << 2)
#define RENDERMODE_FILL              (1 << 3)
#define RENDERMODE_TEXTURE           (1 << 4)
#define RENDERMODE_TEXTURE_WIREFRAME (1 << 5)
#define RENDERMODE_CULL							 (1 << 6)

namespace Starlight {
	namespace Render {

		enum RendererType : u8 {
			RendererType_Software,
			RendererType_DirectX11,
			RendererType_DirectX12,
			RendererType_Vulkan,
			RendererType_Metal,
			RendererType_Count
		};
		
		struct Renderer {
			union {
				struct {
					void* memory_buffer;
					u32* colour_buffer;
					f32* z_buffer;
					s32 bytes_per_pixel;
					s32 pitch;
					s32 height;
					s32 width;

					#if defined(OS_WINDOWS)
						BITMAPINFO win32_bitmapinfo;
					#endif
				} sw;
			};

			RendererType type; // Contigent, I'd prefer this to be more explicit depending on the Platform, etc.
			u8 render_mode;
		};

		// OS - Renderer Association
		internal void allocate_buffer(Arena *arena, Renderer *buffer, s32 width, s32 height);
		internal void copy_buffer_to_window(Handle window_handle, Renderer *buffer);
		internal void clear_colour_buffer(Renderer *buffer, u32 colour);
		internal void clear_z_buffer(Renderer *buffer);

		//internal RenderHandle window_equip(Handle window);
	}
}
