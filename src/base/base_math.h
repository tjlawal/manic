// Copyright Frost Gorilla, Inc. All Rights Reserved.

#ifndef BASE_MATH_H
#define BASE_MATH_H

//- tijani: Miscl defines
#define PI 3.14159265

// Vector ops

// NOTE(tijani): Vertices are just points;
typedef union Vec2F32 Vec2F32;
union Vec2F32 {
  struct {
    f32 x;
    f32 y;
  };
  f32 v[2];
};

typedef union Vec2S32 Vec2S32;
union Vec2S32 {
  struct {
    s32 x;
    s32 y;
  };
  s32 v[2];
};

typedef struct Vec3F32 Vec3F32;
struct Vec3F32 {
  f32 x;
  f32 y;
  f32 z;
};

typedef struct Vec4F32 Vec4F32;
struct Vec4F32 {
  f32 x;
  f32 y;
  f32 z;
  f32 w;
};

// 2D
internal f32 vec2_length(Vec2F32 vector);
internal Vec2F32 vec2_add(Vec2F32 a, Vec2F32 b);
internal Vec2F32 vec2_sub(Vec2F32 a, Vec2F32 b);
internal Vec2F32 vec2_mul(Vec2F32 vector, f32 factor);
internal Vec2F32 vec2_div(Vec2F32 vector, f32 factor);
internal f32 vec2_dot(Vec2F32 a, Vec2F32 b);
internal Vec2F32 vec2f32_from_vec4f32(Vec4F32 v);

// 3D
internal f32 vec3_length(Vec3F32 vector);
internal void vec3_normalize(Vec3F32 *v);
internal f32 vec3_dot(Vec3F32 a, Vec3F32 b);
internal Vec3F32 vec3_add(Vec3F32 a, Vec3F32 b);
internal Vec3F32 vec3_sub(Vec3F32 a, Vec3F32 b);
internal Vec3F32 vec3_mul(Vec3F32 vector, f32 factor);
internal Vec3F32 vec3_div(Vec3F32 vector, f32 factor);
internal Vec3F32 vec3_cross(Vec3F32 a, Vec3F32 b);
internal Vec3F32 vec3f32_rotate_x(Vec3F32 vector, f32 angle);
internal Vec3F32 vec3f32_rotate_y(Vec3F32 vector, f32 angle);
internal Vec3F32 vec3f32_rotate_z(Vec3F32 vector, f32 angle);
internal Vec3F32 vec3f32_from_vec4f32(Vec4F32 v);

// 4D
internal Vec4F32 vec4f32_from_vec3f32(Vec3F32 v);

// Vector ops macros
// NOTE(tijani): These macros allow the functions to be used as types.
#define V2S32(x, y) vec2_s32((x), (y))

// Matrix

// 4 x 4
typedef struct Mat4F32 Mat4F32;
struct Mat4F32 {
  f32 m[4][4];
};

internal Mat4F32 mat4f32_identity(void);
internal Mat4F32 mat4f32_scale(f32 x, f32 y, f32 z);
internal Mat4F32 mat4f32_translate(f32 tx, f32 ty, f32 tz);
internal Mat4F32 mat4f32_rotate_x(f32 x);
internal Mat4F32 mat4f32_rotate_y(f32 y);
internal Mat4F32 mat4f32_rotate_z(f32 z);
internal Vec4F32 mat4f32_mul_vec4(Mat4F32 m, Vec4F32 v);
internal Mat4F32 mat4f32_mul_mat4f32(Mat4F32 a, Mat4F32 b);
internal Vec4F32 mat4f32_mul_projection(Mat4F32 projection_matrix, Vec4F32 v);
internal Mat4F32 mat4f32_perspective_project(f32 fov, f32 aspect_ratio, f32 znear, f32 zfar);

// Range Types

// 1-range
typedef union Rng1u64 Rng1u64;
union Rng1u64 {
  struct {
    u64 min;
    u64 max;
  };

  u64 v[2];
};

// Range ops
internal Rng1u64 rng1u64(u64 min, u64 max);
internal u64 rngdiff1u64(Rng1u64 rng);

// NOTE(tijani): These macros allow the functions to be used as types.

#define r1u64(min, max) rng_1u64((min), (max));

//// TODO(tijani): Revise these, they shouldn't be in the math header!
//- tijani: Textures
typedef struct Texture2F32 Texture2F32;
struct Texture2F32 {
  f32 u;
  f32 v;
};

//- tijani: Triangles
typedef struct Triangle2F32 Triangle2F32;
struct Triangle2F32 {
  Vec4F32 points[3];
  Texture2F32 texture_coords[3];
  u32 colour;
};

//- tijani: Faces are the surfaces formed by connecting those points.
typedef struct Face3S32 Face3S32;
struct Face3S32 {
  s32 a;
  s32 b;
  s32 c;
  Texture2F32 a_uv;
  Texture2F32 b_uv;
  Texture2F32 c_uv;
  u32 colour;
};

// Calculate the barycentric weights of alpha, beta and gamma for point p;
internal Vec3F32 barycentric_weights(Vec2F32 a, Vec2F32 b, Vec2F32 c, Vec2F32 p);

#endif // BASE_MATH_H
