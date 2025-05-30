#pragma once

#define MATH_PI 3.141592653589793238462643383279502884197169399375f

namespace Starlight {
	namespace Foundation {
		// Forward declare stuff so order is irrelevant
		union Vec2F32;
		union Vec2S32;
		union Vec3F32;
		union Vec4F32;
		union Rng1u64;
		struct Mat4F32;

		// Vectors
		union Vec2F32 {
			struct {
				f32 x;
				f32 y;
			};

			f32 v[2];

			Vec2F32() : x(0), y(0) {}
			Vec2F32(f32 _x, f32 _y) : x(_x), y(_y) {}

			Vec2F32 operator+(const Vec2F32& left) const { return Vec2F32 { x + left.x, y + left.y }; }
			Vec2F32 operator-(const Vec2F32& left) const { return Vec2F32 { x - left.x, y - left.y }; }
			Vec2F32 operator*(const Vec2F32& left) const { return Vec2F32 { x * left.x, y * left.y }; }
			Vec2F32 operator/(const Vec2F32& left) const { return Vec2F32 { x / left.x, y / left.y }; }

			internal f32 dot(Vec2F32 a, Vec2F32 b); 
			internal f32 length(Vec2F32 vector); 
			internal Vec2F32 vec2f32_from_vec4f32(Vec4F32 vector); 
		};

		union Vec2S32 {
			struct {
				s32 x;
				s32 y;
			};
			s32 v[2];

			Vec2S32() : x(0), y(0) {}
			Vec2S32(s32 _x, s32 _y) : x(_x), y(_y) {}

			Vec2S32 operator+(const Vec2S32& left) const;
			Vec2S32 operator-(const Vec2S32& left) const;
			Vec2S32 operator*(const Vec2S32& left) const;
			Vec2S32 operator/(const Vec2S32& left) const;

			internal f32 dot(Vec2F32 a, Vec2F32 b);
			internal f32 length(Vec2F32 vector);
		};

		union Vec3F32 {
			struct {
				f32 x;
				f32 y;
				f32 z;
			};
			f32 v[3];

			Vec3F32() : x(0), y(0), z(0) {}
			Vec3F32(f32 _x, f32 _y, f32 _z) : x(_x), y(_y), z(_z) {}

			Vec3F32 operator+(const Vec3F32& left) const { return Vec3F32 { x + left.x, y + left.y, z + left.z }; }
			Vec3F32 operator-(const Vec3F32& left) const { return Vec3F32 { x - left.x, y - left.y, z - left.z }; }
			Vec3F32 operator*(const Vec3F32& left) const { return Vec3F32 { x * left.x, y * left.y, z * left.z }; }
			Vec3F32 operator/(const Vec3F32& left) const { return Vec3F32 { x / left.x, y / left.y, z / left.z }; }

			internal f32 dot(Vec3F32 a, Vec3F32 b);
			internal f32 length(Vec3F32 v);
			internal void normalize(Vec3F32* v);
			internal Vec3F32 cross(Vec3F32 a, Vec3F32 b);

			internal Vec3F32 rotate_x(Vec3F32 vector, f32 angle);
			internal Vec3F32 rotate_y(Vec3F32 vector, f32 angle);
			internal Vec3F32 rotate_z(Vec3F32 vector, f32 angle);

			internal Vec3F32 vec3f32_from_vec4f32(Vec4F32);
		};

		union Vec4F32 {
			struct {
				f32 x;
				f32 y;
				f32 z;
				f32 w;
			};
			f32 v[4];

			Vec4F32() : x(0), y(0), z(0), w(0) {}
			Vec4F32(f32 _x, f32 _y, f32 _z, f32 _w) : x(_x), y(_y), z(_z), w(_w) {}

			Vec4F32 operator+(const Vec4F32& left) const { return Vec4F32 { x + left.x, y + left.y, z + left.z, w + left.w }; }
			Vec4F32 operator-(const Vec4F32& left) const { return Vec4F32 { x - left.x, y - left.y, z - left.z, w - left.w }; }
			Vec4F32 operator*(const Vec4F32& left) const { return Vec4F32 { x * left.x, y * left.y, z * left.z, w * left.w }; }
			Vec4F32 operator/(const Vec4F32& left) const { return Vec4F32 { x / left.x, y / left.y, z / left.z, w / left.w }; }

			internal Vec4F32 vec4f32_from_vec3f32(Vec3F32 v);
		};

		// Ranges
		// 1-Dimension
		union Rng1u64 {
			struct {
				u64 minimum;
				u64 maximum;
			};
			u64 r[2];

			Rng1u64() : minimum(0), maximum(0) {}
			Rng1u64(u64 _min, u64 _max) : minimum(_min), maximum(_max) {}
		};

		internal u64 rng_diff1u64(Rng1u64 rng);

		// Matrices
		// 4 x 4
		struct Mat4F32 {
			f32 m[4][4];

			Mat4F32 operator+(const Mat4F32& left) const {}
			Mat4F32 operator-(const Mat4F32& left) const {}
			Mat4F32 operator*(const Mat4F32& left) const {}
			Mat4F32 operator/(const Mat4F32& left) const {}

			// REVISE: A second pass should be done on these functions for performance. Not necessary now cause 
			// its just the foundation.

			FORCE_INLINE internal Mat4F32 identity(void) {
				// | 1 0 0 0 |
				// | 0 1 0 0 |
				// | 0 0 1 0 |
				// | 0 0 0 1 |

				Mat4F32 m = {{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}};
				return m;
			}

			FORCE_INLINE internal Mat4F32 scale(f32 x, f32 y, f32 z) {
				// |  x 0 0 0 |
				// |  0 y 0 0 |
				// |  0 0 z 0 |
				// |  0 0 0 1 |

				Mat4F32 m = identity();
				m.m[0][0] = x;
				m.m[1][1] = y;
				m.m[2][2] = z;
				return m;
			}

			FORCE_INLINE internal Mat4F32 translate(f32 tx, f32 ty, f32 tz) {
				// |1 0 0 tx|			|x|					|x + tx|
				// |0 1 0 ty|   	|y|					|y + ty|
				// |0 0 1 tz|	 *	|z|    =		|z + tz|
				// |0 0 0 1 |			|1|					|  1 	 |
				Mat4F32 m = identity();

				m.m[0][3] = tx;
				m.m[1][3] = ty;
				m.m[2][3] = tz;

				return m;
			}

			FORCE_INLINE internal Mat4F32 rotate_x(f32 angle) {
				// Matrix rotation in x-axis
				// |1		0				0			0|			|x|
				// |0  cos(x)	-sin(x)	0|			|y|
				// |0  sin(x)	 cos(x)	0|		*	|z|
				// |0		0	  		0	  	1|			|1|
				f32 l_cos = cos(angle);
				f32 l_sin = sin(angle);

				Mat4F32 m = identity();

				m.m[1][1] = l_cos;
				m.m[1][2] = -l_sin;
				m.m[2][1] = l_sin;
				m.m[2][2] = l_cos;

				return m;
			}

			FORCE_INLINE internal Mat4F32 rotate_y(f32 angle) {
				// |cos(y)	0		sin(y)	0|			|x|
				// |  0   	1		 0			0|			|y|
				// |-sin(y)	0		cos(y)	0|	 * 	|z|
				// |  0			0	   0			1|			|1|

				f32 l_cos = cos(angle);
				f32 l_sin = sin(angle);

				Mat4F32 m = identity();

				m.m[0][0] = l_cos;
				m.m[0][2] = l_sin;
				m.m[2][0] = -l_sin;
				m.m[2][2] = l_cos;

				return m;
			}

			FORCE_INLINE internal Mat4F32 rotate_z(f32 angle) {
				// |cos(x) -sin(x)	0	 0|			|x|
				// |sin(x)  cos(x)	0	 0|			|y|
				// | 0			 0			1	 0|  *  |z|
				// | 0			 0			0	 1|			|1|

				f32 l_cos = cos(angle);
				f32 l_sin = sin(angle);

				Mat4F32 m = identity();

				m.m[0][0] = l_cos;
				m.m[0][1] = -l_sin;
				m.m[1][0] = l_sin;
				m.m[1][1] = l_cos;

				return m;
			}

			FORCE_INLINE internal Vec4F32 mat4f32_mul_vec4(Mat4F32 m, Vec4F32 v) {
				Vec4F32 result;
				result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
				result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
				result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
				result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;

				return result;
			}

			FORCE_INLINE internal Mat4F32 mat4f32_mul_mat4f32(Mat4F32 a, Mat4F32 b) {
				Mat4F32 result;

				for (s32 rows = 0; rows < 4; ++rows) {
					for (s32 cols = 0; cols < 4; ++cols) {
						result.m[rows][cols] = a.m[rows][0] * b.m[0][cols] + a.m[rows][1] * b.m[1][cols] + a.m[rows][2] * b.m[2][cols] +
							a.m[rows][3] * b.m[3][cols];
					}
				}

				return result;
			}

			FORCE_INLINE internal Vec4F32 mat4f32_mul_projection(Mat4F32 projection_matrix, Vec4F32 v) {
				// Multiply the projection matrix by the original vector
				Vec4F32 result = mat4f32_mul_vec4(projection_matrix, v);

				// Perform perspective divide with original z-value that is
				// stored in the projection matrix 'w', hence normalizing the entire image
				// space.
				if (result.w != 0.0) {
					result.x /= result.w;
					result.y /= result.w;
					result.z /= result.w;
				}

				return result;	
			}

			FORCE_INLINE internal Mat4F32 mat4f32_perspective_project(f32 fov, f32 aspect_ratio, f32 znear, f32 zfar) {
				// Matrix projection formula
				// |(h/w)*(1/tan(fov/2)							 0										0														 0|			|x|
				// |									0		1/tan(fov/2)										0														 0|   	|y|
				// | 									0							 0		zfar/(zfar-znear)		(-zfar*znear)/(zfar-znear)|	 *	|z|
				// | 									0							 0										1														 0|			|1|
				Mat4F32 result = {{0}};

				result.m[0][0] = aspect_ratio * (1 / tan(fov / 2));
				result.m[1][1] = (1 / tan(fov / 2));
				result.m[2][2] = (zfar / (zfar - znear));
				result.m[2][3] = (((-zfar) * znear) / (zfar - znear));
				result.m[3][2] = 1.0;

				return result;
			}

		};

		// Textures, Triangles, Faces
		struct Texture2F32 {
			f32 u;
			f32 v;
		};

		struct Triangle2F32 {
			Vec4F32 points[3];
			Texture2F32 texture_coords[3];
			u32 colour;
		};

		// Faces are the surfaces formed by connecting those points.
		struct Face3S32 {
			Texture2F32 a_uv;
			Texture2F32 b_uv;
			Texture2F32 c_uv;
			u32 colour;
			s32 a;
			s32 b;
			s32 c;
		};

		// Calculate the barycentric weights of alpha, beta and gamma for point p;
		FORCE_INLINE internal Vec3F32 barycentric_weights(Vec2F32 a, Vec2F32 b, Vec2F32 c, Vec2F32 p) {
			// Find the vectors between the vertices ABC and point p
			Vec2F32 ac = c - a;
			Vec2F32 ab = b - a;
			Vec2F32 pc = c - p;
			Vec2F32 pb = b - p;
			Vec2F32 ap = p - a;


			// Area of the parallelogram (triangle ABC) using cross product
			f32 area_parallelogram_abc = ((ac.x * ab.y) - (ac.y * ab.x)); // || AC x AB ||

			// Alpha is area of the parallelogram [PBC] over the area of the full parallelogram [ABC]
			f32 alpha = ((pc.x * pb.y) - (pc.y * pb.x)) / area_parallelogram_abc;

			// Beta is area of the parallelogram [APC] over the area of the
			// full parallelogram [ABC]
			f32 beta = ((ac.x * ap.y) - (ac.y * ap.x)) / area_parallelogram_abc;

			f32 gamma = 1.0 - alpha - beta;

			Vec3F32 weights = {alpha, beta, gamma};
			return weights;
		}
	}
}

