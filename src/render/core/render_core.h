#pragma once

using namespace Starlight::Foundation;
using namespace Starlight::Platform;

namespace Starlight {
	namespace Render {

		enum RendererType : u8 {
			RendererType_Software,
			RendererType_DirectX11,
			RendererType_DirectX12,
			RendererType_Vulkan,
			RendererType_OpenGL,
			RendererType_Metal,
			RendererType_Count
		};

		enum RenderMode : u8 {
			RenderMode_WireFrame,
			RenderMode_VertexPoints,
			RenderMode_Fill,
			RenderMode_Texture,
			RenderMode_TextureWireFrame,
			RenderMode_BackCull,
			RenderMode_Count
		};

		enum CullMode : u8 {
			CullMode_Back
		};
		
		struct Renderer {
			union {
				struct {
					void* context;
					s32 major;
					s32 minor;
					b16 core, compatible;
					// DLL handle ??
				} opengl;

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

			RendererType type;
			RenderMode mode;
			CullMode cull;
			u8 padding;
		};

		// OS - Renderer Association
		internal void allocate_buffer(Arena *arena, Renderer *buffer, s32 width, s32 height);
		internal void copy_buffer_to_window(Handle window_handle, Renderer *buffer);
		internal void clear_colour_buffer(Renderer *buffer, u32 colour);
		internal void clear_z_buffer(Renderer *buffer);

		//internal RenderHandle window_equip(Handle window);
	}
}
