
#pragma once

namespace Starlight {
	namespace ResourceManager {
		namespace Parser {

			internal void parse_vertices(Arena* arena, Lexer* lexer, MeshInfo* dst);
			internal void parse_textures(Arena* arena, Lexer* lexer, MeshInfo* dst);
			internal Vertex parse_normals(Arena* arena, Lexer* lexer, MeshInfo* dst);
			internal Face parse_faces(Arena* arena, Lexer* lexer, MeshInfo* dst);

			// Helpers
			internal u32 count_vertices(Token token, Lexer* lexer);
			internal u32 count_normals(Token token, Lexer* lexer);
			internal u32 count_texcoords(Token token, Lexer* lexer);
			internal u32 count_faces(Token token, Lexer* lexer);

			// internal Vec4 parse_geom_vertices(Arena *arena, Lexer* lexer);
			// internal Vec3 parse_texture_vertices(Lexer* lexer);
			// internal Vec3 parse_vertex_normals(Lexer* lexer);
			// internal void parse_face_element(Lexer* lexer);
			// internal void parse_line_element(Lexer* lexer);
			// internal void parse_point_element(Lexer* lexer);
		}
	}
}