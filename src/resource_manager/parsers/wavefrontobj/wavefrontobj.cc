/******************************************************************************
* Todos:
* 
* - Some OBJ files include additional information like material name (usemtl), 
*		material library (mtlib) that is not relevant in this use case so it is
*		skipped over and parsing continues.
*	- It'll be nice to report errors when parsing invalid OBJ files and 
*		exit gracefully.
* - Need to investigate why it parses on really large obj files, but then again 
*		OBJ files are the wrong tool for the job so it's just a good exercise and 
*		not necessary. (see dragon.obj)
*	- Writing back an already parsed OBJ file is trivial but there's no use case.
*
*
******************************************************************************/


namespace Starlight {
	namespace ResourceManager {
		namespace Parser {

			internal MeshInfo*  load_mesh_data(Arena* arena, string8 data) {
				ProfFunction(profDebug_coral);
				Token token = {};
				Lexer lexer(data);
				MeshInfo* mesh_info = arena_push<MeshInfo>(arena, 1);

				{
					ProfScope(count, "count data elements", profDebug_peru);
					mesh_info->vertices_idx = 0;
					mesh_info->normals_idx = 0;
					mesh_info->texture_coords_idx = 0;
					mesh_info->faces_idx = 0;

					mesh_info->vertices_count = count_vertices(token, &lexer);
					mesh_info->normals_count = count_normals(token, &lexer);;
					mesh_info->texture_coords_count = count_texcoords(token, &lexer);
					mesh_info->faces_count = count_faces(token, &lexer);
				}

				{
					ProfScope(alloc_mesh, "alloc data to MeshInfo", profDebug_snow);
					mesh_info->vertices = arena_push<Vec3>(arena, mesh_info->vertices_count);
					mesh_info->normals = arena_push<Vec3>(arena, mesh_info->normals_count);
					mesh_info->texture_coords = arena_push<TextureCoord>(arena, mesh_info->texture_coords_count);
					mesh_info->faces = arena_push<Face>(arena, mesh_info->faces_count);
				}

				{
					ProfScope(start_parsing, "start parseing file", profDebug_bisque);
					while(token.type != FormatTokenType_EOF) {
						token = next_token(&lexer);

						switch(token.type) {
							case(FormatTokenType_GeometricVertices): {
								parse_vertices(arena, &lexer, mesh_info);
							} break;

							case(FormatTokenType_VertexNormals): {
								parse_normals(arena, &lexer, mesh_info);
							} break;

							case(FormatTokenType_TextureVertices): {
								parse_textures(arena, &lexer, mesh_info);
							} break;

							case(FormatTokenType_Face): {
								parse_faces(arena, &lexer, mesh_info);
							} break;
						}
					}
				}

				return mesh_info;
			}

			internal Token next_token(Lexer* lexer) {
				Token token = {};
				read_char(lexer);
				eat_all_whitespace(lexer);
				eat_comments(lexer);

				switch(lexer->current_char) {

					// Format: o object_name
					case('o'): {
						read_char(lexer);
					} break;
					
					// Format: `v x y z`
					case('v'): {
						if(peek_ahead(lexer) == ' ') {
							token.type = FormatTokenType_GeometricVertices;
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
						} else {
							//DEBUGBREAK;
							read_char(lexer);
						}
					} break;

					// Format: `f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3`
					case('f'): {
						if(peek_ahead(lexer) == ' ') {
							token.type = FormatTokenType_Face;
						} else {
							read_char(lexer);
						}	
					} break;

					// Format: mtlib library_name.mtl
					case('m'): {
						if(peek_ahead(lexer) == 't') {
							read_char(lexer);
						}
					} break;

					// Format: usemtl: material_name
					case('u'): {
						if(peek_ahead(lexer) == 's') {
							read_char(lexer);
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
		
			internal void parse_vertices(Arena* arena, Lexer* lexer, MeshInfo* dst) {
				Vec3 vertex = {};
				parse_float(lexer, &vertex.x);
				parse_float(lexer, &vertex.y);
				parse_float(lexer, &vertex.z);

				dst->vertices[dst->vertices_idx++] = vertex;
			}

			internal void parse_normals(Arena* arena, Lexer* lexer, MeshInfo* dst) {
				Vec3 normals = {};
				parse_float(lexer, &normals.x);
				parse_float(lexer, &normals.y);
				parse_float(lexer, &normals.z);

				dst->normals[dst->normals_idx++] = normals;
			}

			internal void parse_textures(Arena* arena, Lexer* lexer, MeshInfo* dst) {
				TextureCoord texture = {};

				parse_float(lexer, &texture.u);
				parse_float(lexer, &texture.v);
				dst->texture_coords[dst->texture_coords_idx++] = texture;
			}

			internal void parse_faces(Arena* arena, Lexer* lexer, MeshInfo* dst) {
				ProfFunction(profDebug_orchid);
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

				// Now we have all the data filled out into their respective vertices, 
				// allocate and

				dst->faces[dst->faces_idx++] = faces;
			}

			
			// Helpers
			internal u32 count_vertices(Token token, Lexer* lexer) {
				ProfFunction(profDebug_blue);
				u32 count = 0;
				while(token.type != FormatTokenType_EOF) {
					token = next_token(lexer);
					switch(token.type) {
						case(FormatTokenType_GeometricVertices): {
							count += 1;
						} break;
					}
				}

				rewind_lexer(lexer);
				return count;
			}

			internal u32 count_normals(Token token, Lexer* lexer) {
				ProfFunction(profDebug_blue);
				u32 count = 0;
				while(token.type != FormatTokenType_EOF) {
					token = next_token(lexer);
					switch(token.type) {
						case(FormatTokenType_VertexNormals): {
							count += 1;
						} break;
					}
				}
				
				rewind_lexer(lexer);
				return count;
			}

			internal u32 count_texcoords(Token token, Lexer* lexer) {
				ProfFunction(profDebug_blue);
				u32 count = 0;
				while(token.type != FormatTokenType_EOF) {
					token = next_token(lexer);
					switch(token.type) {
						case(FormatTokenType_TextureVertices): {
							count += 1;
						} break;
					}
				}
				
				rewind_lexer(lexer);
				return count;
			}

			internal u32 count_faces(Token token, Lexer* lexer) {
				ProfFunction(profDebug_blue);
				u32 count = 0;
				while(token.type != FormatTokenType_EOF) {
					token = next_token(lexer);
					switch(token.type) {
						case(FormatTokenType_Face): {
							count += 1;
						} break;
					}
				}
				
				rewind_lexer(lexer);
				return count;
			}

			internal void parse_float(Lexer *lexer, f32* output_value) {
				ProfFunction(profDebug_ivory);;
				// @TODO: Revise and handle errors or make sure they dont occur at all!
				char buffer[MAX_NUMERIC_BUFFER_SIZE] = {0};
				s32 buffer_index = 0;
				read_char(lexer);
				eat_all_whitespace(lexer);

				// Handle sign 
				ProfScope(sign, "handle sign parsing", profDebug_wheat);
				{
					if(lexer->current_char == '-') {
						buffer[buffer_index++] = lexer->current_char;
						read_char(lexer);
					}
				}

				ProfScope(is_digit_, "check is digit", profDebug_wheat);
				{
					while(is_digit(lexer->current_char) && buffer_index < (MAX_NUMERIC_BUFFER_SIZE - 1)) {
						buffer[buffer_index++] = lexer->current_char;
						read_char(lexer);
					}
				}

				ProfScope(read_decimal, "read decimal", profDebug_wheat);
				{
					// Read decimal point and keep reading after.
					if(lexer->current_char == '.') {
						buffer[buffer_index++] = lexer->current_char;
						read_char(lexer);

						while((is_digit(lexer->current_char)) && (buffer_index < (MAX_NUMERIC_BUFFER_SIZE - 1)) ) {
							buffer[buffer_index++] = lexer->current_char;
							read_char(lexer);
						}
					}
				}

				if(buffer_index >= MAX_NUMERIC_BUFFER_SIZE) {
					// @TODO: Figure out scenario that this could occur and handle the error instead 
					// of crashing the entire application.
					DEBUGBREAK;
					return;
				}
	
				buffer[buffer_index] = '\0';

				// Convert to float
				// @TODO: Revise this, according to Daniel Lemire, strtof is significantly slow.
				// https://lemire.me/blog/2020/03/10/fast-float-parsing-in-practice/
				// But this is kinnda good given that OBJ files would not be used for 3D meshes soon enough.
				ProfScope(c_strof, "strtof conversion", profDebug_wheat);
				char *end;
				*output_value = strtof(buffer, &end);
			}

		}
	}
}

