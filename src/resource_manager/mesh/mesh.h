#pragma once

using namespace Starlight::Foundation;

namespace Starlight {
	namespace ResourceManager {
		namespace Mesh {

			struct MeshInfo {
				Vec3* vertices;
				Vec3* normals;
				Face* faces;
				TextureCoord* texture_coords;

				u32 faces_idx;
				u32 normals_idx;
				u32 vertices_idx;
				u32 texture_coords_idx;

				u32 faces_count;
				u32 normals_count;
				u32 vertices_count;
				u32 texture_coords_count;

				u32 *texture_data;
				s32 texture_width;
				s32 texture_height;
				s32 texture_channels;

				u32 colour;

				// Transformation data ??
				Vec3 rotate;
				Vec3 scale; 
				Vec3 translate;
			};

			internal void dump_mesh_info(MeshInfo* mesh);
		}
	}
}