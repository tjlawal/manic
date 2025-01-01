// Copyright Frost Gorilla, Inc. All Rights Reserved.

#define CUBE_VERTICES 8
#define CUBE_FACES    (6 * 2) // 6 cube faces, 2 triangles per face

// Globals
// @revise: this is sus. could there be a better way?
Mesh g_mesh = {.vertices = NULL, .faces = NULL, .rotate = {0, 0, 0}, .scale = {1.0, 1.0, 1.0}, .translate = {0, 0, 0}};

// clang-format off
Vec3F32 cube_vertices[CUBE_VERTICES] = {
  { .x = -1, .y = -1, .z = -1},
  { .x = -1, .y =  1, .z = -1},
  { .x =  1, .y =  1, .z = -1},
  { .x =  1, .y = -1, .z = -1},
  { .x =  1, .y =  1, .z =  1},
  { .x =  1, .y = -1, .z =  1},
  { .x = -1, .y =  1, .z =  1},
  { .x = -1, .y = -1, .z =  1},
};
// clang-format on

Face3S32 cube_faces[CUBE_FACES] = {
    // front
    {.a = 1, .b = 2, .c = 3, .colour = 0xFFFF0000},
    {.a = 1, .b = 3, .c = 4, .colour = 0xFFFF0000},

    // right
    {.a = 4, .b = 3, .c = 5, .colour = 0xFF00FF00},
    {.a = 4, .b = 5, .c = 6, .colour = 0xFF00FF00},

    // back
    {.a = 6, .b = 5, .c = 7, .colour = 0xFF0000FF},
    {.a = 6, .b = 7, .c = 8, .colour = 0xFF0000FF},

    // left
    {.a = 8, .b = 7, .c = 2, .colour = 0xFFFFFF00},
    {.a = 8, .b = 2, .c = 1, .colour = 0xFFFFFF00},

    // top
    {.a = 2, .b = 7, .c = 5, .colour = 0xFFFF00FF},
    {.a = 2, .b = 5, .c = 3, .colour = 0xFFFF00FF},

    // bottom
    {.a = 6, .b = 8, .c = 1, .colour = 0xFF00FFFF},
    {.a = 6, .b = 1, .c = 4, .colour = 0xFF00FFFF},
};

void load_cube_mesh_data(void) {
  for (int i = 0; i < CUBE_VERTICES; i++) {
    Vec3F32 cube_vertex = cube_vertices[i];
    array_push(g_mesh.vertices, cube_vertex);
  }

  for (int i = 0; i < CUBE_FACES; i++) {
    Face3S32 cube_face = cube_faces[i];
    array_push(g_mesh.faces, cube_face);
  }
}

void load_obj_file_data_from_file(char *filename) {
  FILE *file_ptr = fopen(filename, "r");

  char buffer[2048];
  while (fgets(buffer, sizeof buffer, file_ptr)) {
    // Vertex information
    if (strncmp(buffer, "v ", 2) == 0) {
      Vec3F32 vertex;
      sscanf(buffer, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
      array_push(g_mesh.vertices, vertex);
    }

    // Faces information
    if (strncmp(buffer, "f ", 2) == 0) {
      // Face3S32 faces;
      int vertex_indices[3];
      int texture_indices[3];
      int normal_indices[3];

      sscanf(buffer, "f %d/%d/%d %d/%d/%d %d/%d/%d", &vertex_indices[0], &texture_indices[0], &normal_indices[0],
             &vertex_indices[1], &texture_indices[1], &normal_indices[1], &vertex_indices[2], &texture_indices[2],
             &normal_indices[2]);

      Face3S32 faces = {.a = vertex_indices[0], .b = vertex_indices[1], .c = vertex_indices[2]};

      sscanf(buffer, "f %d %d %d", &faces.a, &faces.b, &faces.c);
      array_push(g_mesh.faces, faces);
    }
  }
}
