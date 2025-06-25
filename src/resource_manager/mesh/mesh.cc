
namespace Starlight {
	namespace ResourceManager {
		namespace Mesh {

			internal void dump_mesh_info(MeshInfo* mesh_info) {
				if (!mesh_info) {
					printf("MeshInfo is NULL\n");
					return;
				}

				printf("=== MeshInfo Dump ===\n");
    
				// Counts and indices
				printf("Vertices Count: %u (Current Index: %u)\n", mesh_info->vertices_count, mesh_info->vertices_idx);
				printf("Normals Count: %u (Current Index: %u)\n", mesh_info->normals_count, mesh_info->normals_idx);
				printf("Texture Coords Count: %u (Current Index: %u)\n", mesh_info->texture_coords_count, mesh_info->texture_coords_idx);
				printf("Faces Count: %u\n", mesh_info->faces_count);
    
				// Texture info
				printf("Texture Size: %dx%d\n", mesh_info->texture_width, mesh_info->texture_height);
				printf("Texture Data: %s\n", mesh_info->texture_data ? "Present" : "NULL");
    
				// Transformation data
				printf("Translation: (%.3f, %.3f, %.3f)\n", mesh_info->translate.x, mesh_info->translate.y, mesh_info->translate.z);
				printf("Rotation: (%.3f, %.3f, %.3f)\n", mesh_info->rotate.x, mesh_info->rotate.y, mesh_info->rotate.z);
				printf("Scale: (%.3f, %.3f, %.3f)\n", mesh_info->scale.x, mesh_info->scale.y, mesh_info->scale.z);
    
				printf("\n");
    
				// Dump vertices
				if (mesh_info->vertices && mesh_info->vertices_count > 0) {
					printf("--- Vertices ---\n");
					for (u32 i = 0; i < mesh_info->vertices_count; i++) {
						printf("v[%u]: (%.6f, %.6f, %.6f)\n", i, 
									 mesh_info->vertices[i].x, 
									 mesh_info->vertices[i].y, 
									 mesh_info->vertices[i].z);
					}
					printf("\n");
				}

				// Dump texture coordinates
				if (mesh_info->texture_coords && mesh_info->texture_coords_count > 0) {
					printf("--- Texture Coordinates ---\n");
					for (u32 i = 0; i < mesh_info->texture_coords_count; i++) {
						printf("vt[%u]: (%.6f, %.6f)\n", i,
									 mesh_info->texture_coords[i].u,
									 mesh_info->texture_coords[i].v);
					}
					printf("\n");
				}
    
				// Dump normals
				if (mesh_info->normals && mesh_info->normals_count > 0) {
					printf("--- Normals ---\n");
					for (u32 i = 0; i < mesh_info->normals_count; i++) {
						printf("vn[%u]: (%.6f, %.6f, %.6f)\n", i,
									 mesh_info->normals[i].x,
									 mesh_info->normals[i].y,
									 mesh_info->normals[i].z);
					}
					printf("\n");
				}
    
				// Dump faces
				if (mesh_info->faces && mesh_info->faces_count > 0) {
					printf("--- Faces ---\n");
					for (u32 i = 0; i < mesh_info->faces_count; i++) {
						printf("f[%u]: %d/%d/%d %d/%d/%d %d/%d/%d\n", i,
									 mesh_info->faces[i].vertex_idx[0], mesh_info->faces[i].texture_idx[0], mesh_info->faces[i].normal_idx[0],
									 mesh_info->faces[i].vertex_idx[1], mesh_info->faces[i].texture_idx[1], mesh_info->faces[i].normal_idx[1],
									 mesh_info->faces[i].vertex_idx[2], mesh_info->faces[i].texture_idx[2], mesh_info->faces[i].normal_idx[2]);
					}
					printf("\n");
				}
    
				printf("=== End MeshInfo Dump ===\n\n");
			}
		}
	}
}