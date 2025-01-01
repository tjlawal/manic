// Copyright Frost Gorilla, Inc. All Rights Reserved.

// Perspective Projection (perspective divide)
// Formula
//   P'x = Px/Pz
//   P'y = Py/Pz
Vec2F32 perspective_projection(Vec3F32 points) {
  Vec2F32 projected_point = {.x = (field_of_view_factor * points.x) / points.z,
                             .y = (field_of_view_factor * points.y) / points.z};
  return projected_point;
}