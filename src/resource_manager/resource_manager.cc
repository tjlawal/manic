
#include "parsers/parser.cc"
#include "parsers/wavefrontobj/wavefrontobj.cc"

using namespace Starlight::Platform;
using namespace Starlight::ResourceManager::Parser;

namespace Starlight {
	namespace ResourceManager {
		
		internal void* load_model(Arena* arena, string8 file_path) {
			// Read data from file
			// Parse data in to a MeshInfo
			// return MeshInfo to renderer
			
			string8 data = read_data_from_file(arena, file_path);
			void* parsed_data = rm_parse_data_from_file(arena, data);

			return parsed_data;
			
		}


	}
}