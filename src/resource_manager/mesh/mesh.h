#pragma once

using namespace Starlight::Foundation;

namespace Starlight {
	namespace ResourceManager {
		namespace Mesh {

			struct MeshInfo {
				Face* 	faces;
				Vertex* vertices;
				Vertex* normals;
				TextureCoord* texture_coords;

				u32 vertices_count;
				u32 normals_count;
				u32 faces_count;
				u32 texture_coords_count;

				// Transformation data
				Vec3 rotate;
				Vec3 scale; 
				Vec3 translate;

				// Optional texture data
				u32 *texture_data;
				s32 texture_width;
				s32 texture_height;
			};


		}
	}
}