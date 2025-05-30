#pragma once

using namespace Starlight::Foundation;

namespace Starlight {
	namespace Render {

		enum RendererType : u8 {
			RendererType_Software,
			RendererType_DirectX,
			RendererType_Vulkan,
			RendererType_OpenGL,
			RendererType_Metal,

			RendererType_Count
		};
		
		struct Renderer {
			RendererType type;
			union {
				struct {
					void* context;
					s32 major;
					s32 minor;
					b16 core, compatible;
					// DLL handle ??
				} opengl;

				struct {
					#if defined(OS_WINDOWS)
						BITMAPINFO win32_bitmapinfo;
					#endif
					void* memory;
					u32* colour_buffer;
					f32* z_buffer;
					s32 bytes_per_pixel;
					s32 pitch;
					s32 height;
					s32 width;
				} software_framebuffer;
			};

		};

		internal void r_resize_buffer(Arena *arena, Renderer *buffer, s32 width, s32 height);
		internal void r_copy_buffer_to_window(HDC device_context, Renderer *buffer);
		internal void r_clear_colour_buffer(Renderer *buffer, u32 colour);
		internal void r_clear_z_buffer(Renderer *buffer);

	}
}
