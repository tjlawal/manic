
using namespace Starlight::Foundation;
using namespace Starlight::Platform::Gfx;

namespace Starlight {
	namespace Render {

		void allocate_buffer(Arena *arena, Renderer *buffer, s32 width, s32 height) {
			ProfFunction(profDebug_steelblue);
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
			buffer->sw.z_buffer = reinterpret_cast<f32*>(mem_block + colour_buffer_size); // Offset cause its stored contiguously alongside colour_buffer!
		}

		void copy_buffer_to_window(Handle window_handle, Renderer *buffer) {
			ProfFunction(profDebug_dodgerblue);
			Window* window = w32_window_from_handle(window_handle);
			HDC hdc = window->hdc;
			StretchDIBits(hdc, 0, 0, buffer->sw.width, buffer->sw.height, 0, 0, 
										buffer->sw.width, buffer->sw.height, buffer->sw.colour_buffer, 
										&buffer->sw.win32_bitmapinfo, DIB_RGB_COLORS, SRCCOPY);
		}

		// @REVISE: This could be faster!!
		void clear_colour_buffer(Renderer *buffer, u32 colour) {
			ProfFunction(profDebug_firebrick);
			for (s32 y = 0; y < buffer->sw.height; y++) {
				for (s32 x = 0; x < buffer->sw.width; x++) {
					buffer->sw.colour_buffer[(buffer->sw.width * y) + x] = colour;
				}
			}
		}

		// @REVISE: This could be faster!!
		void clear_z_buffer(Renderer *buffer) {
			ProfFunction(profDebug_dimgrey);
			for (s32 y = 0; y < buffer->sw.height; ++y) {
				for (s32 x = 0; x < buffer->sw.width; ++x) {
					buffer->sw.z_buffer[(buffer->sw.width * y) + x] = 1.0f;
				}
			}
		}

	
	}
}