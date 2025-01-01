// Copyright Frost Gorilla, Inc. All Rights Reserved.

// 2D Vector Operations

// Signed
internal Vec2S32 vec2_s32(s32 x, s32 y) {
  Vec2S32 v = {x, y};
  return v;
}

f32 vec2_length(Vec2F32 vector) { return (f32)sqrt(vector.x * vector.x + vector.y * vector.y); }

Vec2F32 vec2_add(Vec2F32 a, Vec2F32 b) {
  Vec2F32 result = {.x = a.x + b.x, .y = a.y + b.y};

  return result;
}
Vec2F32 vec2_sub(Vec2F32 a, Vec2F32 b) {
  Vec2F32 result = {.x = a.x - b.x, .y = a.y - b.y};
  return result;
}

Vec2F32 vec2_mul(Vec2F32 vector, f32 factor) {
  Vec2F32 result = {.x = vector.x * factor, .y = vector.y * factor};
  return result;
}

Vec2F32 vec2_div(Vec2F32 vector, f32 factor) {
  Vec2F32 result = {.x = vector.x / factor, .y = vector.y / factor};
  return result;
}

internal f32 vec2_dot(Vec2F32 a, Vec2F32 b) { return ((a.x * b.x) + (a.y * b.y)); }

// 3D Vector Operations
f32 vec3_length(Vec3F32 vector) { return (f32)sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z); }

Vec3F32 vec3_add(Vec3F32 a, Vec3F32 b) {
  Vec3F32 result = {.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z};
  return result;
}
Vec3F32 vec3_sub(Vec3F32 a, Vec3F32 b) {
  Vec3F32 result = {.x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z};
  return result;
}

Vec3F32 vec3_mul(Vec3F32 vector, f32 factor) {
  Vec3F32 result = {.x = vector.x * factor, .y = vector.y * factor, .z = vector.z * factor};
  return result;
}

Vec3F32 vec3_div(Vec3F32 vector, f32 factor) {
  Vec3F32 result = {.x = vector.x / factor, .y = vector.y / factor, .z = vector.z / factor};
  return result;
}

Vec3F32 vec3_cross(Vec3F32 a, Vec3F32 b) {
  Vec3F32 result = {.x = (a.y * b.z) - (a.z * b.y), .y = (a.z * b.x) - (a.x * b.z), .z = (a.x * b.y) - (a.y * b.x)};
  return result;
}

// Dot Product:
// a*b = axbx + ayby
internal f32 vec3_dot(Vec3F32 a, Vec3F32 b) { return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z)); }

Vec3F32 vec3f32_rotate_x(Vec3F32 vector, f32 new_angle) {
  Vec3F32 rotated_vector = {.x = vector.x,
                            .y = vector.y * (f32)cos(new_angle) - vector.z * (f32)sin(new_angle),
                            .z = vector.y * (f32)sin(new_angle) + vector.z * (f32)cos(new_angle)};
  return rotated_vector;
}

Vec3F32 vec3f32_rotate_y(Vec3F32 vector, f32 new_angle) {
  Vec3F32 rotated_vector = {.x = vector.x * (f32)cos(new_angle) - vector.z * (f32)sin(new_angle),
                            .y = vector.y,
                            .z = vector.x * (f32)sin(new_angle) + vector.z * (f32)cos(new_angle)};
  return rotated_vector;
}

Vec3F32 vec3f32_rotate_z(Vec3F32 vector, f32 new_angle) {
  Vec3F32 rotated_vector = {.x = vector.x * (f32)cos(new_angle) - vector.y * (f32)sin(new_angle),
                            .y = vector.x * (f32)sin(new_angle) + vector.y * (f32)cos(new_angle),
                            .z = vector.z};
  return rotated_vector;
}