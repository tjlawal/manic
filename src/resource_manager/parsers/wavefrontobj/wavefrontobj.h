
#pragma once

namespace Starlight {
	namespace ResourceManager {
		namespace Parser {

			internal void count_mesh_elements(Token token, Lexer* lexer, u32 vert_count, u32 norm_count, u32 face_count, u32 texcoord_count);
			internal Vertex parse_vertices(Arena* arena, Lexer* lexer, MeshInfo* dst);
			internal TextureCoord parse_textures(Arena* arena, Lexer* lexer, MeshInfo* dst);
			internal Vertex parse_normals(Arena* arena, Lexer* lexer, MeshInfo* dst);
			internal Face parse_faces(Arena* arena, Lexer* lexer, MeshInfo* dst);

			// internal Vec4 parse_geom_vertices(Arena *arena, Lexer* lexer);
			// internal Vec3 parse_texture_vertices(Lexer* lexer);
			// internal Vec3 parse_vertex_normals(Lexer* lexer);
			// internal void parse_face_element(Lexer* lexer);
			// internal void parse_line_element(Lexer* lexer);
			// internal void parse_point_element(Lexer* lexer);
		}
	}
}