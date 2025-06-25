#pragma once

using namespace Starlight::Foundation;

namespace Starlight {
	namespace ResourceManager {
		namespace Mesh {

			struct MeshInfo {
				Vertex* vertices;
				Vertex* normals;
				TextureCoord* texture_coords;
				Face* 	faces;

				u32 vertices_idx;
				u32 normals_idx;
				u32 texture_coords_idx;
				u32 faces_idx;

				u32 vertices_count;
				u32 normals_count;
				u32 texture_coords_count;
				u32 faces_count;

				u32 *texture_data;
				s32 texture_width;
				s32 texture_height;

				// Transformation data ??
				Vec3 rotate;
				Vec3 scale; 
				Vec3 translate;
			};

			internal void dump_mesh_info(MeshInfo* mesh);
		}
	}
}