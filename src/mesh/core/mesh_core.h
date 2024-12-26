// Copyright Frost Gorilla, Inc. All Rights Reserved.

#ifndef MESH_CORE_H
#define MESH_CORE_H

typedef struct Mesh Mesh;
struct Mesh {
  Vec3F32 *vertices; // dynamic array of vertices
  Face3S32 *faces;   // dynamic array of faces
  Vec3F32 rotation;  // rotation with x, y, and z values
};

void load_cube_mesh_data(void);
void load_obj_file_data_from_file(char *filename);

#endif // MESH_CORE_H
