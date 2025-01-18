// Copyright Frost Gorilla, Inc. All Rights Reserved.
// clang-format off

// 2D Vector Operations

// Signed
internal Vec2S32 vec2_s32(s32 x, s32 y) {
  Vec2S32 v = {x, y};
  return v;
}

internal f32 vec2_length(Vec2F32 vector) { return (f32)sqrt(vector.x * vector.x + vector.y * vector.y); }

internal Vec2F32 vec2_add(Vec2F32 a, Vec2F32 b) {
  Vec2F32 result = {.x = a.x + b.x, .y = a.y + b.y};

  return result;
}
internal Vec2F32 vec2_sub(Vec2F32 a, Vec2F32 b) {
  Vec2F32 result = {.x = a.x - b.x, .y = a.y - b.y};
  return result;
}

internal Vec2F32 vec2_mul(Vec2F32 vector, f32 factor) {
  Vec2F32 result = {.x = vector.x * factor, .y = vector.y * factor};
  return result;
}

internal Vec2F32 vec2_div(Vec2F32 vector, f32 factor) {
  Vec2F32 result = {.x = vector.x / factor, .y = vector.y / factor};
  return result;
}

internal f32 vec2_dot(Vec2F32 a, Vec2F32 b) { return ((a.x * b.x) + (a.y * b.y)); }

// 3D Vector Operations
internal f32 vec3_length(Vec3F32 vector) {
  return (f32)sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

internal Vec3F32 vec3_add(Vec3F32 a, Vec3F32 b) {
  Vec3F32 result = {.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z};
  return result;
}
internal Vec3F32 vec3_sub(Vec3F32 a, Vec3F32 b) {
  Vec3F32 result = {.x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z};
  return result;
}

internal Vec3F32 vec3_mul(Vec3F32 vector, f32 factor) {
  Vec3F32 result = {.x = vector.x * factor, .y = vector.y * factor, .z = vector.z * factor};
  return result;
}

internal Vec3F32 vec3_div(Vec3F32 vector, f32 factor) {
  Vec3F32 result = {.x = vector.x / factor, .y = vector.y / factor, .z = vector.z / factor};
  return result;
}

internal void vec3_normalize(Vec3F32 *v) {
  f32 length = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);

  v->x /= length;
  v->y /= length;
  v->z /= length;
}

internal Vec3F32 vec3_cross(Vec3F32 a, Vec3F32 b) {
  Vec3F32 result = {
		.x = (a.y * b.z) - (a.z * b.y), 
		.y = (a.z * b.x) - (a.x * b.z), 
		.z = (a.x * b.y) - (a.y * b.x)
	};
  return result;
}

// Dot Product:
// a*b = axbx + ayby
internal f32 vec3_dot(Vec3F32 a, Vec3F32 b) { return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z)); }

internal Vec3F32 vec3f32_rotate_x(Vec3F32 vector, f32 new_angle) {
  Vec3F32 rotated_vector = {.x = vector.x,
                            .y = vector.y * (f32)cos(new_angle) - vector.z * (f32)sin(new_angle),
                            .z = vector.y * (f32)sin(new_angle) + vector.z * (f32)cos(new_angle)};
  return rotated_vector;
}

internal Vec3F32 vec3f32_rotate_y(Vec3F32 vector, f32 new_angle) {
  Vec3F32 rotated_vector = {.x = vector.x * (f32)cos(new_angle) - vector.z * (f32)sin(new_angle),
                            .y = vector.y,
                            .z = vector.x * (f32)sin(new_angle) + vector.z * (f32)cos(new_angle)};
  return rotated_vector;
}

internal Vec3F32 vec3f32_rotate_z(Vec3F32 vector, f32 new_angle) {
  Vec3F32 rotated_vector = {.x = vector.x * (f32)cos(new_angle) - vector.y * (f32)sin(new_angle),
                            .y = vector.x * (f32)sin(new_angle) + vector.y * (f32)cos(new_angle),
                            .z = vector.z};
  return rotated_vector;
}

// tijani: Vec4F32

Vec4F32 vec4f32_from_vec3f32(Vec3F32 v) {
  Vec4F32 result = {v.x, v.y, v.z, 1.0};
  return result;
}

internal Vec3F32 vec3f32_from_vec4f32(Vec4F32 v) {
  Vec3F32 result = {v.x, v.y, v.z};
  return result;
}

// tijani: Mat 4x4

internal Mat4F32 mat4f32_identity(void) {
  // | 1 0 0 0 |
  // | 0 1 0 0 |
  // | 0 0 1 0 |
  // | 0 0 0 1 |

  Mat4F32 m = {{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}};
  return m;
}

internal Mat4F32 mat4f32_scale(f32 x, f32 y, f32 z) {
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

internal Mat4F32 mat4f32_translate(f32 tx, f32 ty, f32 tz) {
  // |1 0 0 tx|			|x|					|x + tx|
  // |0 1 0 ty|   	|y|					|y + ty|
  // |0 0 1 tz|	 *	|z|    =		|z + tz|
  // |0 0 0 1 |			|1|					|
  // 1	 |
  Mat4F32 m = mat4f32_identity();

  m.m[0][3] = tx;
  m.m[1][3] = ty;
  m.m[2][3] = tz;

  return m;
}


// Matrix rotation in x-axis
// |1		0				0			0|			|x|
// |0  cos(x)	-sin(x)	0|			|y|
// |0  sin(x)	 cos(x)	0|		*	|z|
// |0		0	  		0	  	1|			|1|
internal Mat4F32 mat4f32_rotate_x(f32 angle) {
  f32 l_cos = cos(angle);
  f32 l_sin = sin(angle);

  Mat4F32 m = mat4f32_identity();

  m.m[1][1] = l_cos;
  m.m[1][2] = -l_sin;
  m.m[2][1] = l_sin;
  m.m[2][2] = l_cos;

  return m;
}

internal Mat4F32 mat4f32_rotate_y(f32 angle) {
  // |cos(y)	0		sin(y)	0|			|x|
  // |  0   	1		 0			0|			|y|
  // |-sin(y)	0		cos(y)	0|	 * 	|z|
  // |  0			0	   0			1|			|1|

  f32 l_cos = cos(angle);
  f32 l_sin = sin(angle);

  Mat4F32 m = mat4f32_identity();

  m.m[0][0] = l_cos;
  m.m[0][2] = l_sin;
  m.m[2][0] = -l_sin;
  m.m[2][2] = l_cos;

  return m;
}

internal Mat4F32 mat4f32_rotate_z(f32 angle) {
  // |cos(x) -sin(x)	0	 0|			|x|
  // |sin(x)  cos(x)	0	 0|			|y|
  // | 0			 0			1	 0|  *  |z|
  // | 0			 0			0	 1|			|1|

  f32 l_cos = cos(angle);
  f32 l_sin = sin(angle);

  Mat4F32 m = mat4f32_identity();

  m.m[0][0] = l_cos;
  m.m[0][1] = -l_sin;
  m.m[1][0] = l_sin;
  m.m[1][1] = l_cos;

  return m;
}

internal Vec4F32 mat4f32_mul_vec4(Mat4F32 m, Vec4F32 v) {
  Vec4F32 result;

  result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
  result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
  result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
  result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;

  return result;
}

internal Mat4F32 mat4f32_mul_mat4f32(Mat4F32 a, Mat4F32 b) {
  Mat4F32 result;

  for (s32 rows = 0; rows < 4; ++rows) {
    for (s32 cols = 0; cols < 4; ++cols) {
      result.m[rows][cols] = a.m[rows][0] * b.m[0][cols] + a.m[rows][1] * b.m[1][cols] + a.m[rows][2] * b.m[2][cols] +
                             a.m[rows][3] * b.m[3][cols];
    }
  }

  return result;
}

internal Vec4F32 mat4f32_mul_projection(Mat4F32 projection_matrix, Vec4F32 v) {
  // Multiply the projection matrix by the original vector
  Vec4F32 result = mat4f32_mul_vec4(projection_matrix, v);

  // NOTE(tijani): Perform perspective divide with original z-value that is
  // stored in the projection matrix 'w', hence normalizing the entire image
  // space.

  if (result.w != 0.0) {
    result.x /= result.w;
    result.y /= result.w;
    result.z /= result.w;
  }

  return result;
}

// NOTE(tijani): Camera projection matrix.
// Maybe appropriate to move this into the camera projection code?
internal Mat4F32 mat4f32_perspective_project(f32 fov, f32 aspect_ratio, f32 znear, f32 zfar) {
  // Matrix projection formula
  // clang-format off
	// |(h/w)*(1/tan(fov/2)							 0										0														 0|			|x|
	// |									0		1/tan(fov/2)										0														 0|   	|y|
	// | 									0							 0		zfar/(zfar-znear)		(-zfar*znear)/(zfar-znear)|	 *	|z|
	// | 									0							 0										1														 0|			|1|
  // clang-format on
  Mat4F32 result = {{0}};

  result.m[0][0] = aspect_ratio * (1 / tan(fov / 2));
  result.m[1][1] = (1 / tan(fov / 2));
  result.m[2][2] = (zfar / (zfar - znear));
  result.m[2][3] = (((-zfar) * znear) / (zfar - znear));
  result.m[3][2] = 1.0;

  return result;
}

// Calculato the barycentric weights of alpha, beta and gamma for point p;
internal Vec3F32 barycentric_weights(Vec2F32 a, Vec2F32 b, Vec2F32 c, Vec2F32 p) {
  // Find the vectors between the vertices ABC and point p
  Vec2F32 ac = vec2_sub(c, a);
  Vec2F32 ab = vec2_sub(b, a);
  Vec2F32 pc = vec2_sub(c, p);
  Vec2F32 pb = vec2_sub(b, p);
  Vec2F32 ap = vec2_sub(p, a);

  // NOTE(tijani): Area of the parallelogram (triangle ABC) using cross product
  f32 area_parallelogram_abc = ((ac.x * ab.y) - (ac.y * ab.x)); // || AC x AB ||

  // NOTE(tijani): Alpha is area of the parallelogram [PBC] over the area of the
  // full parallelogram [ABC]
  f32 alpha = ((pc.x * pb.y) - (pc.y * pb.x)) / area_parallelogram_abc;

  // NOTE(tijani): Beta is area of the parallelogram [APC] over the area of the
  // full parallelogram [ABC]
  f32 beta = ((ac.x * ap.y) - (ac.y * ap.x)) / area_parallelogram_abc;

  f32 gamma = 1.0 - alpha - beta;

  Vec3F32 weights = {alpha, beta, gamma};
  return weights;
}
