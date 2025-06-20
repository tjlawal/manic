namespace Starlight {
	namespace ResourceManager {
		namespace Parser {

			internal MeshInfo*  rm_parse_data_from_file(Arena* arena, string8 data) {
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
							parse_textures(arena, &lexer, mesh_info);
						} break;

						case(FormatTokenType_VertexNormals): {
							parse_normals(arena, &lexer, mesh_info);
						} break;

						case(FormatTokenType_Face): {
							parse_faces(arena, &lexer, mesh_info);
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

					// Format: `f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3`
					case('f'): {
						if(peek_ahead(lexer) == ' ') {
							token.type = FormatTokenType_Face;
						} else {
							// Invalid OBJ file, report error!
							DEBUGBREAK;
						}	
					} break;
					
					default: {
						if(is_eof(lexer->current_char)) {
							token.type = FormatTokenType_EOF;
							token.literal = &lexer->current_char;
						} else {
							token.type = FormatTokenType_Unknown;
							token.literal = &lexer->current_char;
						}
					} break;
				
				}

				return token;
			}
		
			internal Vertex parse_vertices(Arena* arena, Lexer* lexer, MeshInfo* dst) {
				Vertex vertex = {};
				parse_float(lexer, &vertex.x);
				parse_float(lexer, &vertex.y);
				parse_float(lexer, &vertex.z);

				dst->vertices_count += 1;

				#if BUILD_DEBUG
					printf("v %g %g %g\n", vertex.x, vertex.y, vertex.z);
				#endif
				return vertex;
			}

			internal TextureCoord parse_textures(Arena* arena, Lexer* lexer, MeshInfo* dst) {
				TextureCoord texture = {};

				parse_float(lexer, &texture.u);
				parse_float(lexer, &texture.v);

				#if BUILD_DEBUG
				printf("vt %g %g\n", texture.u, texture.v);
				#endif
				return texture;
			}

			internal Vertex parse_normals(Arena* arena, Lexer* lexer, MeshInfo* dst) {
				Vertex normals = {};

				parse_float(lexer, &normals.x);
				parse_float(lexer, &normals.y);
				parse_float(lexer, &normals.z);

				dst->normals_count += 1;

				#if BUILD_DEBUG
				printf("vn %g %g %g\n", normals.x, normals.y, normals.z);
				#endif
				return normals;
			}

			internal Face parse_faces(Arena* arena, Lexer* lexer, MeshInfo* dst) {
				Face faces = {};
				char buffer[MAX_NUMERIC_BUFFER_SIZE] = {0};
				s32 buffer_index = 0;

				read_char(lexer);
				eat_all_whitespace(lexer);

				// Parsing the first parts of vertices for this particular face
				{
					{
						// Parse v1 and store in the v[0]
						if(is_digit(lexer->current_char)) {
							while((lexer->current_char != '/') && buffer_index < (MAX_NUMERIC_BUFFER_SIZE - 1)) {
								buffer[buffer_index++] = lexer->current_char;
								read_char(lexer);
							}
						}
						buffer[buffer_index] = '\0';
						faces.vertex_idx[0] = atoi(buffer);
					}


					// Parse vt1 and store in the vt[0]
					{
						MemoryZero(buffer, MAX_NUMERIC_BUFFER_SIZE);
						buffer_index = 0;
						read_char(lexer);
						if(is_digit(lexer->current_char)) {
							while((lexer->current_char != '/') && buffer_index < (MAX_NUMERIC_BUFFER_SIZE - 1)) {
								buffer[buffer_index++] = lexer->current_char;
								read_char(lexer);
							}
						}
						buffer[buffer_index] = '\0';
						faces.texture_idx[0] = atoi(buffer);
					}
		
					// Parse vn1 and store in the vn[0]
					{
						MemoryZero(buffer, MAX_NUMERIC_BUFFER_SIZE);
						buffer_index = 0;
						read_char(lexer);
						if(is_digit(lexer->current_char)) {
							while(!is_whitespace(lexer->current_char)) {
								buffer[buffer_index++] = lexer->current_char;
								read_char(lexer);
							}
						}
						faces.normal_idx[0] = atoi(buffer);
					}
				}

				// Parsing the second parts of vertices for this particular face
				{
					// Parse v2 and store in the v[1]
					{
						MemoryZero(buffer, MAX_NUMERIC_BUFFER_SIZE);
						buffer_index = 0;
						read_char(lexer);
						// Parse v1 and store in the v[0]
						if(is_digit(lexer->current_char)) {
							while((lexer->current_char != '/') && buffer_index < (MAX_NUMERIC_BUFFER_SIZE - 1)) {
								buffer[buffer_index++] = lexer->current_char;
								read_char(lexer);
							}
						}
						buffer[buffer_index] = '\0';
						faces.vertex_idx[1] = atoi(buffer);
					}


					// Parse vt2 and store in the vt[1]
					{
						MemoryZero(buffer, MAX_NUMERIC_BUFFER_SIZE);
						buffer_index = 0;
						read_char(lexer);
						if(is_digit(lexer->current_char)) {
							while((lexer->current_char != '/') && buffer_index < (MAX_NUMERIC_BUFFER_SIZE - 1)) {
								buffer[buffer_index++] = lexer->current_char;
								read_char(lexer);
							}
						}
						buffer[buffer_index] = '\0';
						faces.texture_idx[1] = atoi(buffer);
					}
		
					// Parse vn2 and store in the vv[1]
					{
						MemoryZero(buffer, MAX_NUMERIC_BUFFER_SIZE);
						buffer_index = 0;
						read_char(lexer);
						if(is_digit(lexer->current_char)) {
							while(!is_whitespace(lexer->current_char)) {
								buffer[buffer_index++] = lexer->current_char;
								read_char(lexer);
							}
						}
						faces.normal_idx[1] = atoi(buffer);
					}
				}

				// Parsing the third parts of vertices for this particular face
				{
					// Parse v3 and store in the v[2]
					{
						MemoryZero(buffer, MAX_NUMERIC_BUFFER_SIZE);
						buffer_index = 0;
						read_char(lexer);
						if(is_digit(lexer->current_char)) {
							while((lexer->current_char != '/') && buffer_index < (MAX_NUMERIC_BUFFER_SIZE - 1)) {
								buffer[buffer_index++] = lexer->current_char;
								read_char(lexer);
							}
						}
						buffer[buffer_index] = '\0';
						faces.vertex_idx[2] = atoi(buffer);
					}


					// Parse vt3 and store in the vt[2]
					{
						MemoryZero(buffer, MAX_NUMERIC_BUFFER_SIZE);
						buffer_index = 0;
						read_char(lexer);
						if(is_digit(lexer->current_char)) {
							while((lexer->current_char != '/') && buffer_index < (MAX_NUMERIC_BUFFER_SIZE - 1)) {
								buffer[buffer_index++] = lexer->current_char;
								read_char(lexer);
							}
						}
						buffer[buffer_index] = '\0';
						faces.texture_idx[2] = atoi(buffer);
					}
		
					// Parse vn3 and store in the vn[2]
					{
						MemoryZero(buffer, MAX_NUMERIC_BUFFER_SIZE);
						buffer_index = 0;
						read_char(lexer);
						if(is_digit(lexer->current_char)) {
							while(!is_whitespace(lexer->current_char) && !is_eol(lexer->current_char)) {
								buffer[buffer_index++] = lexer->current_char;
								read_char(lexer);
							}
						}
						faces.normal_idx[2] = atoi(buffer);
					}
				}

				dst->faces_count += 1;

				// Now we have all the data filled out into their respective vertices, 
				// allocate and 

				#if BUILD_DEBUG
				printf("f %d/%d/%d %d/%d/%d %d/%d/%d\n", faces.vertex_idx[0], faces.texture_idx[0], faces.normal_idx[0], faces.vertex_idx[1], faces.texture_idx[1], faces.normal_idx[1], faces.vertex_idx[2], faces.texture_idx[2], faces.normal_idx[2]);

				#endif
			}

			
			// Helpers
			internal void parse_float(Lexer *lexer, f32* output_value) {
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

					while((is_digit(lexer->current_char)) && (buffer_index < (MAX_NUMERIC_BUFFER_SIZE - 1)) ) {
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
				// @TODO: Revise this, according to Daniel Lemire, strtof is significantly slow.
				// https://lemire.me/blog/2020/03/10/fast-float-parsing-in-practice/
				char *end;
				*output_value = strtof(buffer, &end);
			}

		}
	}
}

