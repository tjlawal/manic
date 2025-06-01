
using namespace Starlight::Foundation;

namespace Starlight {
	namespace Render {

		void r_resize_buffer(Arena *arena, Renderer *buffer, s32 width, s32 height) {
			buffer->sw.width = width;
			buffer->sw.height = height;
			buffer->sw.bytes_per_pixel = 4;
			buffer->sw.win32_bitmapinfo.bmiHeader.biSize = sizeof(buffer->sw.win32_bitmapinfo.bmiHeader);
			buffer->sw.win32_bitmapinfo.bmiHeader.biWidth = buffer->sw.width;
			buffer->sw.win32_bitmapinfo.bmiHeader.biHeight = buffer->sw.height; // This would be negative when going top-down
			buffer->sw.win32_bitmapinfo.bmiHeader.biPlanes = 1;
			buffer->sw.win32_bitmapinfo.bmiHeader.biBitCount = 32;
			buffer->sw.win32_bitmapinfo.bmiHeader.biCompression = BI_RGB;

			s32 pixel_count = buffer->sw.width * buffer->sw.height;
			s32 colour_buffer_size = pixel_count * sizeof(u32); // 4 bytes per pixel
			s32 z_buffer_size = pixel_count * sizeof(f32); // 4 bytes per depth value
			s32 total_buffer_size = colour_buffer_size + z_buffer_size;

			// One allocation
			buffer->sw.memory_buffer = arena_push<u8>(arena, total_buffer_size);
			u8* mem_block = static_cast<u8*>(buffer->sw.memory_buffer);
			// Concerned parties get their share!
			buffer->sw.colour_buffer = reinterpret_cast<u32*>(mem_block);
			// Offset cause its stored contiguously alongside colour_buffer!
			buffer->sw.z_buffer = reinterpret_cast<f32*>(mem_block + colour_buffer_size);
		}

		void r_copy_buffer_to_window(HDC device_context, Renderer *buffer) {
			StretchDIBits(device_context, 0, 0, buffer->sw.width, buffer->sw.height, 0, 0, 
										buffer->sw.width, buffer->sw.height, buffer->sw.colour_buffer, 
										&buffer->sw.win32_bitmapinfo, DIB_RGB_COLORS, SRCCOPY);
		}

		// REVISE: Could this be faster?
		void r_clear_colour_buffer(Renderer *buffer, u32 colour) {
			for (s32 y = 0; y < buffer->sw.height; y++) {
				for (s32 x = 0; x < buffer->sw.width; x++) {
					buffer->sw.colour_buffer[(buffer->sw.width * y) + x] = colour;
				}
			}
		}

		// REVISE: Could this be faster?
		void r_clear_z_buffer(Renderer *buffer) {
			for (s32 y = 0; y < buffer->sw.height; ++y) {
				for (s32 x = 0; x < buffer->sw.width; ++x) {
					buffer->sw.z_buffer[(buffer->sw.width * y) + x] = 1.0f;
				}
			}
		}

	
	}
}