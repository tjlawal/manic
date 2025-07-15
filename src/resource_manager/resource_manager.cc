#include "mesh/mesh.cc"
#include "parsers/parser.cc"
#include "parsers/wavefrontobj/wavefrontobj.cc"

using namespace Starlight::Platform;
using namespace Starlight::ResourceManager::Parser;

namespace Starlight {
	namespace ResourceManager {
		
		// Parse model info and its texture data.
		// @NOTE: Only Wavefront objs are supported for now.
		internal MeshInfo* load_model(Arena* arena, string8 file_path) {
			ProfFunction(profDebug_mediumslateblue);
			Temp scratch = scratch_begin(0, 0);
			string8 data = read_data_from_file(scratch.arena, file_path);
			MeshInfo* parsed_data = rm_parse_data_from_file(arena, data);
			scratch_end(scratch);
			return parsed_data;
		}


		internal void* load_font(Arena *arena, string8 file) {
			Temp scratch = scratch_begin(0, 0);
			string8 data = read_data_from_file(scratch.arena, file);
			// parse font file

			scratch_end(scratch);

			// return parsed font data for rendering.
		}
	}
}