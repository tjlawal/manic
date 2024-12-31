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

Vec3F32 vec3f32_from_vec4f32(Vec4F32 v) {
  Vec3F32 result = {v.x, v.y, v.z};
  return result;
}

// tijani: Vec4F32
Vec4F32 vec4f32_from_vec3f32(Vec3F32 v) {
  Vec4F32 result = {v.x, v.y, v.z, 1.0};
  return result;
}

// tijani: Mat 4x4

Mat4F32 mat4f32_identity(void) {
  // | 1 0 0 0 |
  // | 0 1 0 0 |
  // | 0 0 1 0 |
  // | 0 0 0 1 |

  Mat4F32 m = {{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}};
  return m;
}

Mat4F32 mat4f32_make_scale(f32 x, f32 y, f32 z) {
  // |  x 0 0 0 |
  // |  0 y 0 0 |
  // |  0 0 z 0 |
  // |  0 0 0 1 |

  Mat4F32 m = mat4f32_identity();
  m.m[0][0] = x;
  m.m[1][1] = y;
  m.m[2][2] = z;
  return m;
}

Mat4F32 mat4f32_translate(f32 tx, f32 ty, f32 tz) {
  // |1 0 0 tx|			|x|					|x + tx|
  // |0 1 0 ty|   	|y|					|y + ty|
  // |0 0 1 tz|	 *	|z|    =		|z + tz|
  // |0 0 0 1 |			|1|					|	 1	 |
  Mat4F32 m = mat4f32_identity();

  m.m[0][3] = tx;
  m.m[1][3] = ty;
  m.m[2][3] = tz;

  return m;
}

Vec4F32 mat4f32_mul_vec4(Mat4F32 m, Vec4F32 v) {
  Vec4F32 result;

  result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
  result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
  result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
  result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;

  return result;
}
