
#include "mesh/mesh.cc"
#include "parsers/parser.cc"
#include "parsers/wavefrontobj/wavefrontobj.cc"

using namespace Starlight::Platform;
using namespace Starlight::ResourceManager::Parser;

namespace Starlight {
	namespace ResourceManager {
		
		// Parse model info and its texture data.
		// @NOTE: This only works for Wavefront OBJ files.
		// @MEMORY CLEANUP, @TODO: clear data from memory after parsing, no need to keep it 
		// around since it already exist in the MeshInfo struct.
		internal MeshInfo* load_model(Arena* arena, string8 file_path) {
			string8 data = read_data_from_file(arena, file_path);
			MeshInfo* parsed_data = rm_parse_data_from_file(arena, data);
			return parsed_data;			
		}


	}
}