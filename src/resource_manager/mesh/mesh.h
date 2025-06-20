#pragma once

using namespace Starlight::Foundation;

namespace Starlight {
	namespace ResourceManager {
		namespace Mesh {

			struct MeshInfo {
				Vertex* vertices;
				TextureCoord* texture_coords;
				Vertex* normals;
				Face* 	faces;

				u32 vertices_count;
				u32 normals_count;
				u32 faces_count;
				u32 texture_coords_count;

				u32 *texture_data;
				s32 texture_width;
				s32 texture_height;

				// Transformation data ??
				Vec3 rotate;
				Vec3 scale; 
				Vec3 translate;

			};


		}
	}
}