// Copyright Frost Gorilla, Inc. All Rights Reserved.

// Orthographic Projection - convert 3D points into a 2D projected point
Vec2F32 orthographic_projection(Vec3F32 points) {
  Vec2F32 projected_point = {.x = (field_of_view_factor * points.x), .y = (field_of_view_factor * points.y)};
  return projected_point;
}