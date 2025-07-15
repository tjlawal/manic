#include "mesh/mesh.cc"
#include "parsers/parser.cc"
#include "parsers/wavefrontobj/wavefrontobj.cc"

using namespace Starlight::Platform;
using namespace Starlight::ResourceManager::Parser;

namespace Starlight {
	namespace ResourceManager {
		
		// Parse model info and its texture data.
		// @NOTE: Only Wavefront objs are supported for now.

		internal MeshInfo* load_model(Arena* arena, string8 mesh, string8 texture) {
			ProfFunction(profDebug_mediumslateblue);
			Temp scratch = scratch_begin(0, 0);
			MeshInfo* parsed_data = {};
			string8 data = read_data_from_file(scratch.arena, mesh);
			parsed_data = load_mesh_data(arena, data);
			load_texture_data(parsed_data, texture);	
			scratch_end(scratch);
			return parsed_data;
		}

		internal void load_texture_data(MeshInfo* mesh, string8 data) {
			ProfFunction(profDebug_navajowhite);

			mesh->texture_data = reinterpret_cast<u32*>(stbi_load(reinterpret_cast<const char*>(data.str), &mesh->texture_width, 
																														&mesh->texture_height, &mesh->texture_channels, STBI_rgb_alpha));
			Assert(mesh->texture_data == NULL);

		}

		internal void load_font(Arena *arena, string8 file) {
			Temp scratch = scratch_begin(0, 0);
			string8 data = read_data_from_file(scratch.arena, file);
			// parse font file

			scratch_end(scratch);

			// return parsed font data for rendering.
		}
	}
}