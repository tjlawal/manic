// Copyright Frost Gorilla, Inc. All Rights Reserved.

#ifndef MESH_CORE_H
#define MESH_CORE_H

typedef struct Mesh Mesh;
struct Mesh {
  Vec3F32 *vertices; // dynamic array of vertices
  Face3S32 *faces;   // dynamic array of faces
  Vec3F32 rotate;    // rotation with x, y, and z values
  Vec3F32 scale;     // scale with x, y, and z values
  Vec3F32 translate; // translate with x, y, and z values
};

internal void load_cube_mesh_data(void);
internal void load_obj_file_data_from_file(char *filename);
internal void load_texture_data(char *filename);

#endif // MESH_CORE_H
