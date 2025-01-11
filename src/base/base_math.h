// Copyright Frost Gorilla, Inc. All Rights Reserved.

#ifndef BASE_MATH_H
#define BASE_MATH_H

//- tijani: Miscl defines
#define PI 3.14159265

//- tijani: Vertices are points;
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

// 2D Vector operations
internal f32 vec2_length(Vec2F32 vector);
internal Vec2F32 vec2_add(Vec2F32 a, Vec2F32 b);
internal Vec2F32 vec2_sub(Vec2F32 a, Vec2F32 b);
internal Vec2F32 vec2_mul(Vec2F32 vector, f32 factor);
internal Vec2F32 vec2_div(Vec2F32 vector, f32 factor);
internal f32 vec2_dot(Vec2F32 a, Vec2F32 b);

// NOTE(tijani): These macros allow the functions to be used as types.
#define V2S32(x, y) vec2_s32((x), (y))
#define V2F32(x, y) Vec2F32((x), (y))

// 3D Vector operations
internal f32 vec3_length(Vec3F32 vector);
internal Vec3F32 vec3_add(Vec3F32 a, Vec3F32 b);
internal Vec3F32 vec3_sub(Vec3F32 a, Vec3F32 b);
internal Vec3F32 vec3_mul(Vec3F32 vector, f32 factor);
internal Vec3F32 vec3_div(Vec3F32 vector, f32 factor);
internal void vec3_normalize(Vec3F32 *v);
internal Vec3F32 vec3_cross(Vec3F32 a, Vec3F32 b);
internal f32 vec3_dot(Vec3F32 a, Vec3F32 b);
internal Vec3F32 vec3f32_rotate_x(Vec3F32 vector, f32 angle);
internal Vec3F32 vec3f32_rotate_y(Vec3F32 vector, f32 angle);
internal Vec3F32 vec3f32_rotate_z(Vec3F32 vector, f32 angle);
internal Vec3F32 vec3f32_from_vec4f32(Vec4F32 v);

//- tijani: Vec4
internal Vec4F32 vec4f32_from_vec3f32(Vec3F32 v);

//- tijani: Textures
typedef struct TextureF32 TextureF32;
struct TextureF32 {
  f32 u;
  f32 v;
};

//- tijani: Triangles
typedef struct Triangle2F32 Triangle2F32;
struct Triangle2F32 {
  Vec2F32 points[3];
  TextureF32 texture_coords[3];
  u32 colour;
  f32 average_depth;
};

//- tijani: Faces are the surfaces formed by connecting those points.
typedef struct Face3S32 Face3S32;
struct Face3S32 {
  s32 a;
  s32 b;
  s32 c;
  TextureF32 a_uv;
  TextureF32 b_uv;
  TextureF32 c_uv;
  u32 colour;
};

//- tijani: 4 x 4 Matrices

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

#endif // BASE_MATH_H
