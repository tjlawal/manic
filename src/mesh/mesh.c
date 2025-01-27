// Copyright Frost Gorilla, Inc. All Rights Reserved.

// Globals
Mesh g_mesh = {
	.vertices = NULL,
  .faces = NULL,
  //.rotate = {0, 1.57, 1.57},
  .rotate = {0, 0, 0},
  .scale = {1.0, 1.0, 1.0},
  .translate = {0, 0, 0} // NOTE(tijani): Revise, this is sus. could there be a better way ?
}; 

// TODO: Maybe don't pass textures and dimensions all around, 
// find a better way to do it?
global u32 *g_texture_mesh;
global s32 g_texture_width;
global s32 g_texture_height;
global s32 g_channels;

void load_obj_file_data_from_file(char *filename) {
	// TODO: switch to os layer primitive
	// TODO: Make this obj parser more robust, currently it will crash on certain types of .objs
  FILE *file_ptr = fopen(filename, "r");

	char buffer[8192];
  Texture2F32 *texture_coords = NULL;

  while (fgets(buffer, sizeof buffer, file_ptr)) {
    // Vertex information
    if (strncmp(buffer, "v ", 2) == 0) {
      Vec3F32 vertex;
      sscanf(buffer, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
      array_push(g_mesh.vertices, vertex);
    }

    // Texture Coordinate information
    if (strncmp(buffer, "vt ", 3) == 0) {
      Texture2F32 texture_coord;
      sscanf(buffer, "vt %f %f", &texture_coord.u, &texture_coord.v);
      array_push(texture_coords, texture_coord);
    }

    // Faces information
    if (strncmp(buffer, "f ", 2) == 0) {
      // Face3S32 faces;
      s32 vertex_indices[3];
      s32 texture_indices[3];
      s32 normal_indices[3];

      sscanf(buffer, "f %d/%d/%d %d/%d/%d %d/%d/%d", &vertex_indices[0], &texture_indices[0], &normal_indices[0],
             &vertex_indices[1], &texture_indices[1], &normal_indices[1], &vertex_indices[2], &texture_indices[2],
             &normal_indices[2]);

      // NOTE(tijani): OBJ starts from 1 not zero, hence why the substractions to account for that.
      Face3S32 face = {.a = vertex_indices[0],
                       .b = vertex_indices[1],
                       .c = vertex_indices[2],
                       .a_uv = texture_coords[texture_indices[0] - 1],
                       .b_uv = texture_coords[texture_indices[1] - 1],
                       .c_uv = texture_coords[texture_indices[2] - 1],
                       .colour = 0xFFFFFFFF};

      // sscanf(buffer, "f %d %d %d", &faces.a, &faces.b, &faces.c);
      array_push(g_mesh.faces, face);
    }
  }
  array_free(texture_coords);
}

internal void load_texture_data(char *filename) {
  s32 width;
  s32 height;
  s32 channels;

  g_texture_mesh = (u32 *)stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
  Assert(g_texture_mesh != NULL);

  g_texture_width = width;
  g_texture_height = height;
  g_channels = channels;

  //stbi_image_free(g_texture_mesh);
}