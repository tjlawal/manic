
#pragma once

namespace Starlight {
	namespace ResourceManager {
		namespace Parser {

			internal Vec4 parse_geom_vertices(Arena *arena, Lexer* lexer);
			internal Vec3 parse_textutre_vertices(Lexer* lexer);
			internal Vec3 parse_vertex_normals(Lexer* lexer);

			internal void parse_face_element(Lexer* lexer);
			//internal void parse_line_element(Lexer* lexer);
			//internal void parse_point_element(Lexer* lexer);
		}
	}
}