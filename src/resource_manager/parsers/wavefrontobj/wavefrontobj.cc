namespace Starlight {
	namespace ResourceManager {
		namespace Parser {

			internal MeshInfo* rm_parse_data_from_file(Arena* arena, string8 data) {
				Token token = {};
				Lexer lexer(data);
				MeshInfo* mesh_info = arena_push<MeshInfo>(arena, 1);

				while(token.type != FormatTokenType_EOF) {
					token = next_token(arena, &lexer);

					switch(token.type) {
						case(FormatTokenType_GeometricVertices): {
							parse_vertices(arena, &lexer, mesh_info);
						} break;

						case(FormatTokenType_TextureVertices): {
							//parse_textures(arena, &lexer, mesh_info);
						} break;

						case(FormatTokenType_VertexNormals): {
							//parse_normals(arena, &lexer, mesh_info);
						} break;

						case(FormatTokenType_Face): {
							//parse_faces(arena, &lexer, mesh_info);
						} break;

						default: {
							token.type = FormatTokenType_Unknown;
							//token.literal = str8_lit("");
						} break;
					}
				}

				return mesh_info;
			}


			internal Token next_token(Arena* arena, Lexer* lexer) {
				Token token = {};
				read_char(lexer);
				eat_all_whitespace(lexer);
				eat_comments(lexer);

				switch(lexer->current_char) {
					
					// Format: `v x y z`
					case('v'): {
						if(peek_ahead(lexer) == ' ') {
							token.type = FormatTokenType_GeometricVertices;
							//token.literal = ;
						} 
						
						// Format: `vt u v`
						else if(peek_ahead(lexer) == 't') {
							token.type = FormatTokenType_TextureVertices;
							read_char(lexer);
						} 
						
						// Format: `vn i j`
						else if(peek_ahead(lexer) == 'n') {
							token.type = FormatTokenType_VertexNormals;
							read_char(lexer);
						} 

						else {
							// Invalid OBJ file, report error!
							DEBUGBREAK;
						}
					} break;

					//// Format: `f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3`
					//case('f'): {
					//	if(peek_ahead(lexer) == ' ') {
					//		token.type = FormatTokenType_Face;
					//	} else {
					//		// Invalid OBJ file, report error!
					//		DEBUGBREAK;
					//	}
						
					//} break;
					
					//default {
					//	if(is_eof(lexer->current_char)) {
					//		token.type = OBJToken_EOF;
					//		token.literal = &lexer->current_char;
					//	} else {
					//		token.type = OBJToken_Unknown;
					//		token.type = lexer->current_char;
					//	}
					//} break;
				
				}

				return token;
			}
		
			internal Vertex parse_vertices(Arena* arena, Lexer* lexer, MeshInfo* dst) {
				Vertex vertex = {};
	
				parse_float(lexer, &vertex.x);
				parse_float(lexer, &vertex.y);
				parse_float(lexer, &vertex.z);

				#if BUILD_DEBUG
				printf("v %f %f %f\n", vertex.x, vertex.y, vertex.z);
				#endif
				return vertex;
			}

			internal Vertex parse_textures(Arena* arena, Lexer* lexer, MeshInfo* dst) {}

			internal Vertex parse_normals(Arena* arena, Lexer* lexer, MeshInfo* dst) {}

			internal Vertex parse_faces(Arena* arena, Lexer* lexer, MeshInfo* dst) {}

			// Helpers specific to this particular format. Some look deciptively similar but are 
			// different as a result of differences in file format, take note! 
			internal void parse_float(Lexer *lexer, f32 *output_value) {
				// TODO: Revise and handle errors or make sure they dont occur at all!
				char buffer[MAX_NUMERIC_BUFFER_SIZE] = {0};
				s32 buffer_index = 0;

				read_char(lexer);
				eat_all_whitespace(lexer);

				// Handle sign 
				if(lexer->current_char == '-') {
					buffer[buffer_index++] = lexer->current_char;
					read_char(lexer);
				}

				while(is_digit(lexer->current_char) && buffer_index < (MAX_NUMERIC_BUFFER_SIZE - 1)) {
					buffer[buffer_index++] = lexer->current_char;
					read_char(lexer);
				}

				// Read decimal point and keep reading after.
				if(lexer->current_char == '.') {
					buffer[buffer_index++] = lexer->current_char;
					read_char(lexer);

					while((!is_whitespace(lexer->current_char)) && (!is_eol(lexer->current_char)) 
						&& (buffer_index < (MAX_NUMERIC_BUFFER_SIZE - 1))) {
						buffer[buffer_index++] = lexer->current_char;
						read_char(lexer);
					}
				}

				if(buffer_index >= MAX_NUMERIC_BUFFER_SIZE) {
					// @TODO: handle error better!
					DEBUGBREAK;
					return;
				}
	
				buffer[buffer_index] = '\0';

				// Convert to float
				// TODO(tijani): Revise this, according to Daniel Lemire, strtof is significantly slow.
				// https://lemire.me/blog/2020/03/10/fast-float-parsing-in-practice/
				char *end;
				*output_value = strtof(buffer, &end);
			}

		}
	}
}

