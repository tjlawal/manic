namespace Starlight {
	namespace ResourceManager {
		namespace Parser {

			void* rm_parse_data_from_file(Arena* arena, string8 data) {
				Token token = {};
				Lexer lexer(data);
				void* parsed_data = nullptr;

				// start lexing
				while(token.type != FormatTokenType_EOF) {
					token = get_token(&lexer);

					switch(token.type) {
						case(FormatTokenType_GeometricVertices): {
							parse_geom_vertices(arena, &lexer);
						} break;

						case(FormatTokenType_TextureVertices): {
							parse_texture_vertices(&lexer);
						} break;

						case(FormatTokenType_VertexNormals): {
							parse_vertex_normals(&lexer);
						} break;

						case(FormatTokenType_Face): {
							parse_face_element(&lexer);
						} break;

						default: {} break;
					}
				}

				// Return the parsed data
				return parsed_data;
			}

			internal Token get_token(Lexer* lexer) {
				Temp scratch = scratch_begin(0, 0);
				Token token = {};
				read_char(lexer);
				eat_all_whitespace(lexer);
				eat_comments(lexer);

				// Maybe skip comments??

				switch(lexer->current_char){
					// Materials
					case('m'): {
						if(peek_ahead(lexer) == 't'){
							// @TODO: Handles this
							//token.type = FormatTokenType_MaterialLibrary;
							//token.literal = reinterpret_cast<const u8*>("mtli");
						}
					} break;
					// Vertex Data

					// Format: `v x y z w`
					case('v'): {
						if(peek_ahead(lexer) == ' '){
							u8 *new_tok = cstr8_concat(scratch.arena, &lexer->current_char, (u8 *)" ");
							token.literal = new_tok;
							token.type = FormatTokenType_GeometricVertices;
						} 
			
						// Format: `vt u v w`
						else if(peek_ahead(lexer) == 't') {
							u8 *new_tok = cstr8_concat(scratch.arena, &lexer->current_char, (u8 *)"t");
							token.literal = new_tok;
							token.type = FormatTokenType_TextureVertices;
							read_char(lexer); // move the current_char to be on `t`
						}

						// Format: `vn i j k`
						else if(peek_ahead(lexer) == 'n') {
							u8 *new_tok = cstr8_concat(scratch.arena, &lexer->current_char, (u8 *)"n");
							token.literal = new_tok;
							token.type = FormatTokenType_VertexNormals;
							read_char(lexer); // move the current_char to be on `n`
						}
						else {
							// Tap out, invalid obj file.
							DEBUGBREAK;
							break;
						}
					} break;

					// Elements:

					// Format: `f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3`
					case('f'): {
						if(peek_ahead(lexer) == ' ') {
							u8 *new_tok = cstr8_concat(scratch.arena, &lexer->current_char, (u8 *)" ");
							token.literal = new_tok;
							token.type = FormatTokenType_Face;
						} else {
							// Tap out, invalid obj file.
							DEBUGBREAK;
							break;
						}
					} break;

					// Format: `p v1 v2 v3`
					case('p'): {
						if(peek_ahead(lexer) == ' ') {
							u8 *new_tok = cstr8_concat(scratch.arena, &lexer->current_char, (u8 *)" ");
							token.literal = new_tok;
							token.type = FormatTokenType_Point;
						} else {
							// Tap out, invalid obj file.
							DEBUGBREAK;
							break;
						}
					} break;

					// Format: `l v1/vt1 v2/vt2 v3/vt3`
					case('l'): {
						if(peek_ahead(lexer) == ' ') {
							u8 *new_tok = cstr8_concat(scratch.arena, &lexer->current_char, (u8 *)" ");
							token.literal = new_tok;
							token.type = FormatTokenType_Line;
						} else {
							// Tap out, invalid obj file.
							DEBUGBREAK;
							break;
						}
					} break;

					default: {
						if(is_eof(lexer->current_char)) {
							token.type = FormatTokenType_EOF;
							token.literal = &lexer->current_char;
						} else {
							token.type = FormatTokenType_Unknown;
							token.literal = reinterpret_cast<u8*>(lexer->current_char);
						}
					}	break;
				}

				scratch_end(scratch);
				return token;

			}

			// Format: `v x y z w`
			internal Vec4 parse_geom_vertices(Arena *arena, Lexer *lexer) {
				Vec4 vertex = {};
	
				parse_float(lexer, &vertex.x);
				parse_float(lexer, &vertex.y);
				parse_float(lexer, &vertex.z);

				// The `w` is optional 
				if(is_digit(peek_ahead(lexer))){
					parse_float(lexer, &vertex.w);
				}

				#if BUILD_DEBUG
					printf("v %f %f %f %f\n", vertex.x, vertex.y, vertex.z, vertex.w);
				#endif
				return vertex;
			}

			// Format: `vt u v w`
			internal Vec3 parse_texture_vertices(Lexer *lexer) {
				Vec3 vertices = {};
				parse_float(lexer, &vertices.x);
				parse_float(lexer, &vertices.y);

				// `w` is optional
				if(is_digit(peek_ahead(lexer))) {
					parse_float(lexer, &vertices.z);
				}

				#if BUILD_DEBUG
					printf("vt %f %f %f\n", vertices.x, vertices.y, vertices.z);
				#endif
				return vertices;
			}

			// Format: `vn i j k`
			internal Vec3 parse_vertex_normals(Lexer *lexer) {
				Vec3 vertices = {};
				parse_float(lexer, &vertices.x);
				parse_float(lexer, &vertices.y);
				parse_float(lexer, &vertices.z);

				#if BUILD_DEBUG
					printf("vn %f %f %f\n", vertices.x, vertices.y, vertices.z);
				#endif
				return vertices;
			}

			// face: `f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3`
			internal void parse_face_element(Lexer *lexer) {
				Face face_vertices = {0};

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
						face_vertices.vertex_idx[0] = atoi(buffer);
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
						face_vertices.texture_idx[0] = atoi(buffer);
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
						face_vertices.normal_idx[0] = atoi(buffer);
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
						face_vertices.vertex_idx[1] = atoi(buffer);
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
						face_vertices.texture_idx[1] = atoi(buffer);
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
						face_vertices.normal_idx[1] = atoi(buffer);
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
						face_vertices.vertex_idx[2] = atoi(buffer);
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
						face_vertices.texture_idx[2] = atoi(buffer);
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
						face_vertices.normal_idx[2] = atoi(buffer);
					}
				}

				// Now we have all the data filled out into their respective vertices, 
				// bundle them into a struct and return them to our caller

				#if BUILD_DEBUG
					printf("f %d/%d/%d %d/%d/%d %d/%d/%d\n", 
								 face_vertices.vertex_idx[0], face_vertices.texture_idx[0], face_vertices.normal_idx[0], 
								 face_vertices.vertex_idx[1], face_vertices.texture_idx[1], face_vertices.normal_idx[1], 
								 face_vertices.vertex_idx[2], face_vertices.texture_idx[2], face_vertices.normal_idx[2]);
				#endif
	
			}

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