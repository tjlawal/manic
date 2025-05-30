
using namespace Starlight::Foundation;

namespace Starlight {
	namespace Render {
		

		// TODO(tijani): this function is too os specific.
		void r_resize_buffer(Arena *arena, Renderer *buffer, s32 width, s32 height) {
			buffer->software_framebuffer.width = width;
			buffer->software_framebuffer.height = height;
			buffer->software_framebuffer.bytes_per_pixel = 4;
			buffer->software_framebuffer.win32_bitmapinfo.bmiHeader.biSize = sizeof(buffer->software_framebuffer.win32_bitmapinfo.bmiHeader);
			buffer->software_framebuffer.win32_bitmapinfo.bmiHeader.biWidth = buffer->software_framebuffer.width;
			buffer->software_framebuffer.win32_bitmapinfo.bmiHeader.biHeight = buffer->software_framebuffer.height; // This would be negative when going top-down
			buffer->software_framebuffer.win32_bitmapinfo.bmiHeader.biPlanes = 1;
			buffer->software_framebuffer.win32_bitmapinfo.bmiHeader.biBitCount = 32;
			buffer->software_framebuffer.win32_bitmapinfo.bmiHeader.biCompression = BI_RGB;

			s32 bitmap_memory_size = (buffer->software_framebuffer.width * buffer->software_framebuffer.height) * buffer->software_framebuffer.bytes_per_pixel;

			//buffer->memory = push_array(arena, u8, bitmap_memory_size);
			buffer->software_framebuffer.memory = arena_push<u8>(arena, bitmap_memory_size);
			buffer->software_framebuffer.colour_buffer = (u32 *)buffer->software_framebuffer.memory;

			// NOTE(tijani): is this a good way to do this?
			f32 z_buffer_memory_size = (buffer->software_framebuffer.width * buffer->software_framebuffer.height) * buffer->software_framebuffer.bytes_per_pixel;
			//f32 *z_buffer_memory = push_array(arena, f32, z_buffer_memory_size);
			f32* z_buffer_memory = arena_push<f32>(arena, z_buffer_memory_size);

			buffer->software_framebuffer.z_buffer = z_buffer_memory;
		}

		// TODO(tijani): This is too OS specific.
		void r_copy_buffer_to_window(HDC device_context, Renderer *buffer) {
			StretchDIBits(device_context, 0, 0, buffer->software_framebuffer.width, buffer->software_framebuffer.height, 0, 0, 
										buffer->software_framebuffer.width, buffer->software_framebuffer.height, buffer->software_framebuffer.memory, 
										&buffer->software_framebuffer.win32_bitmapinfo, DIB_RGB_COLORS, SRCCOPY);
		}

		// TODO(tijani): This is stupid and could be so much faster.
		void r_clear_colour_buffer(Renderer *buffer, u32 colour) {
			for (s32 y = 0; y < buffer->software_framebuffer.height; y++) {
				for (s32 x = 0; x < buffer->software_framebuffer.width; x++) {
					buffer->software_framebuffer.colour_buffer[(buffer->software_framebuffer.width * y) + x] = colour;
				}
			}
		}

		// TODO(tijani): This is stupid and could be so much faster.
		void r_clear_z_buffer(Renderer *buffer) {
			for (s32 y = 0; y < buffer->software_framebuffer.height; ++y) {
				for (s32 x = 0; x < buffer->software_framebuffer.width; ++x) {
					buffer->software_framebuffer.z_buffer[(buffer->software_framebuffer.width * y) + x] = 1.0f;
				}
			}
		}

	
	}
}