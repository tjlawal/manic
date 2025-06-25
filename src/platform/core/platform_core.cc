
using namespace Starlight::Foundation;

namespace Starlight {
	namespace Platform {
		internal Handle handle_zero(void) { return Handle{0}; }

		internal b32 handle_match(Handle a, Handle b) { return a.handle[0] == b.handle[0]; }

		// File system, these abstract away the system primitives
		internal string8 data_from_file_range(Arena* arena, Handle file, Rng1u64 range) {
			u64 position_pre = arena_position(arena);
			string8 result;
			result.size = sizeof_rng1u(range);
			result.str = arena_push_non_zeroed<u8>(arena, result.size);
			u64 actual_read_size = read_file(file, range, result.str);

			// Trim fat
			if(actual_read_size < result.size) {
				arena_pop_to(arena, position_pre + actual_read_size);
				result.size = actual_read_size;
			}
			return result;
		}

		internal string8 read_data_from_file(Arena* arena, string8 file_path) {
			ProfFunction(profDebug_darkred);
			Handle file = open_file(AccessFlag_Read, file_path);
			FileProperty properties = properties_from_file(file);
			string8 data = data_from_file_range(arena, file, rng1u64(0, properties.size));
			close_file(file);
			return data;
		}

		internal b32 write_data_to_file(string8 file_path, string8 data) {}
		internal b32 append_data_to_file(string8 file_path, string8 data) {}
	}
}