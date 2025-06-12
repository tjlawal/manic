#pragma once

#define MATH_PI 3.141592653589793238462643383279502884197169399375f

namespace Starlight {
	namespace Foundation {
		// Forward declare stuff so order is irrelevant
		struct Vec2f;
		struct Vec2s;
		struct Vec3;
		struct Vec4;
		struct Matrix4;
		struct Rng1u64;
		
		
		// Vectors

		// This is mostly used to represent gfx window.
		struct Vec2s {
			s32 x;
			s32 y;

			Vec2s() : x(0), y(0) {}
			Vec2s(s32 _x, s32 _y) : x(_x), y(_y) {}
		};

		// 2D
		struct Vec2f {
			f32 x;
			f32 y;

			Vec2f() : x(0), y(0) {}
			Vec2f(f32 _x, f32 _y) : x(_x), y(_y) {}

			Vec2f operator+(const Vec2f& right) const { return Vec2f { x + right.x, y + right.y }; }
			Vec2f operator-(const Vec2f& right) const { return Vec2f { x - right.x, y - right.y }; }
			Vec2f operator*(const Vec2f& right) const { return Vec2f { x * right.x, y * right.y }; }
			Vec2f operator/(const Vec2f& right) const { return Vec2f { x / right.x, y / right.y }; }

			Vec2f operator*(f32 scalar) const { return Vec2f { x * scalar, y * scalar }; }
			Vec2f operator/(f32 scalar) const { return Vec2f { x / scalar, y / scalar }; }

			internal Vec2f vec2f_from_vec4(Vec4 v);
		};

		// 3D
		struct Vec3 {
			f32 x;
			f32 y;
			f32 z;

			Vec3() : x(0), y(0), z(0) {}
			Vec3(f32 _x, f32 _y, f32 _z) : x(_x), y(_y), z(_z) {}

			Vec3 operator+(const Vec3& left) const { return Vec3 { x + left.x, y + left.y, z + left.z }; }
			Vec3 operator-(const Vec3& left) const { return Vec3 { x - left.x, y - left.y, z - left.z }; }
			Vec3 operator*(const Vec3& left) const { return Vec3 { x * left.x, y * left.y, z * left.z }; }
			Vec3 operator/(const Vec3& left) const { return Vec3 { x / left.x, y / left.y, z / left.z }; }

			internal Vec3 rotate_x(Vec3 vector, f32 angle);
			internal Vec3 rotate_y(Vec3 vector, f32 angle);
			internal Vec3 rotate_z(Vec3 vector, f32 angle);

			internal Vec3 vec3_from_vec4(Vec4);
		};

		// 4D
		struct Vec4{
			f32 x;
			f32 y;
			f32 z;
			f32 w;

			Vec4() : x(0), y(0), z(0), w(0) {}
			Vec4(f32 _x, f32 _y, f32 _z, f32 _w) : x(_x), y(_y), z(_z), w(_w) {}

			Vec4 operator+(const Vec4& left) const { return Vec4 { x + left.x, y + left.y, z + left.z, w + left.w }; }
			Vec4 operator-(const Vec4& left) const { return Vec4 { x - left.x, y - left.y, z - left.z, w - left.w }; }
			Vec4 operator*(const Vec4& left) const { return Vec4 { x * left.x, y * left.y, z * left.z, w * left.w }; }
			Vec4 operator/(const Vec4& left) const { return Vec4 { x / left.x, y / left.y, z / left.z, w / left.w }; }

			internal Vec4 vec4_from_vec3(Vec3 v);
		};

		// Common vector operations

		FORCE_INLINE internal f32 dot(const Vec2f& a, const Vec2f& b) { return ((a.x * b.x) + (a.y * b.y)); }
		FORCE_INLINE internal f32 dot(const Vec3& a, const Vec3& b) { return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z)); }
		FORCE_INLINE internal f32 dot(const Vec4& a, const Vec4& b) { return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w)); }

		//FORCE_INLINE internal length(Vec2f* v) { return }

		//FORCE_INLINE internal length(Vec3* v) {}
		
		//FORCE_INLINE internal length(Vec4* v) {}


		FORCE_INLINE internal void normalize(Vec2f* v) {
			f32 length = sqrtf((v->x * v->x) + (v->y * v->y));

			v->x /= length;
			v->y /= length;
		}

		FORCE_INLINE internal void normalize(Vec3* v) {
			f32 length = sqrtf((v->x * v->x) + (v->y * v->y) + (v->z * v->z));

			v->x /= length;
			v->y /= length;
			v->z /= length;
		}

		FORCE_INLINE internal void normalize(Vec4* v) {
			f32 length = sqrtf((v->x * v->x) + (v->y * v->y) + (v->z * v->z) + (v->w * v->w));

			v->x /= length;
			v->y /= length;
			v->z /= length;
			v->w /= length;
		}

		FORCE_INLINE internal Vec3 cross(const Vec3& a, const Vec3& b) {
			return Vec3 { (a.y * b.z) - (a.z * b.y), (a.z * b.x) - (a.x * b.z), (a.x * b.y) - (a.y * b.x) };
		}

		// Matrices
		// 4 x 4
		// REVISE, easy clap to speed up using SIMD!
		struct Matrix4 {
			f32 m[4][4];

			Matrix4 operator+(const Matrix4& left) const {}
			Matrix4 operator-(const Matrix4& left) const {}
			Matrix4 operator*(const Matrix4& left) const {}
			Matrix4 operator/(const Matrix4& left) const {}

			// REVISE: A second pass should be done on these functions for performance. Not necessary now cause 
			// its just the foundation.

			FORCE_INLINE internal Matrix4 identity(void) {
				// | 1 0 0 0 |
				// | 0 1 0 0 |
				// | 0 0 1 0 |
				// | 0 0 0 1 |
				Matrix4 m = {{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}};
				return m;
			}

			FORCE_INLINE internal Matrix4 scale(f32 x, f32 y, f32 z) {
				// |  x 0 0 0 |
				// |  0 y 0 0 |
				// |  0 0 z 0 |
				// |  0 0 0 1 |

				Matrix4 m = identity();
				m.m[0][0] = x;
				m.m[1][1] = y;
				m.m[2][2] = z;
				return m;
			}

			FORCE_INLINE internal Matrix4 translate(f32 tx, f32 ty, f32 tz) {
				// |1 0 0 tx|			|x|					|x + tx|
				// |0 1 0 ty|   	|y|					|y + ty|
				// |0 0 1 tz|	 *	|z|    =		|z + tz|
				// |0 0 0 1 |			|1|					|  1 	 |
				Matrix4 m = identity();

				m.m[0][3] = tx;
				m.m[1][3] = ty;
				m.m[2][3] = tz;

				return m;
			}

			FORCE_INLINE internal Matrix4 rotate_x(f32 angle) {
				// Matrix rotation in x-axis
				// |1		0				0			0|			|x|
				// |0  cos(x)	-sin(x)	0|			|y|
				// |0  sin(x)	 cos(x)	0|		*	|z|
				// |0		0	  		0	  	1|			|1|
				f32 l_cos = cos(angle);
				f32 l_sin = sin(angle);

				Matrix4 m = identity();

				m.m[1][1] = l_cos;
				m.m[1][2] = -l_sin;
				m.m[2][1] = l_sin;
				m.m[2][2] = l_cos;

				return m;
			}

			FORCE_INLINE internal Matrix4 rotate_y(f32 angle) {
				// |cos(y)	0		sin(y)	0|			|x|
				// |  0   	1		 0			0|			|y|
				// |-sin(y)	0		cos(y)	0|	 * 	|z|
				// |  0			0	   0			1|			|1|

				f32 l_cos = cos(angle);
				f32 l_sin = sin(angle);

				Matrix4 m = identity();

				m.m[0][0] = l_cos;
				m.m[0][2] = l_sin;
				m.m[2][0] = -l_sin;
				m.m[2][2] = l_cos;

				return m;
			}

			FORCE_INLINE internal Matrix4 rotate_z(f32 angle) {
				// |cos(x) -sin(x)	0	 0|			|x|
				// |sin(x)  cos(x)	0	 0|			|y|
				// | 0			 0			1	 0|  *  |z|
				// | 0			 0			0	 1|			|1|

				f32 l_cos = cos(angle);
				f32 l_sin = sin(angle);

				Matrix4 m = identity();

				m.m[0][0] = l_cos;
				m.m[0][1] = -l_sin;
				m.m[1][0] = l_sin;
				m.m[1][1] = l_cos;

				return m;
			}

			FORCE_INLINE internal Vec4 mat4f32_mul_vec4(Matrix4 m, Vec4 v) {
				Vec4 result;
				result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
				result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
				result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
				result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;

				return result;
			}

			FORCE_INLINE internal Matrix4 mat4f32_mul_mat4f32(Matrix4 a, Matrix4 b) {
				Matrix4 result;

				for (s32 rows = 0; rows < 4; ++rows) {
					for (s32 cols = 0; cols < 4; ++cols) {
						result.m[rows][cols] = a.m[rows][0] * b.m[0][cols] + a.m[rows][1] * b.m[1][cols] + a.m[rows][2] * b.m[2][cols] +
							a.m[rows][3] * b.m[3][cols];
					}
				}

				return result;
			}

			FORCE_INLINE internal Vec4 mat4f32_mul_projection(Matrix4 projection_matrix, Vec4 v) {
				// Multiply the projection matrix by the original vector
				Vec4 result = mat4f32_mul_vec4(projection_matrix, v);

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
		};

		// Ranges

		// 1D
		struct Rng1u64 {
			u64 minimum;
			u64 maximum;

			Rng1u64() : minimum(0), maximum(0) {}
			Rng1u64(u64 _min, u64 _max) : minimum(_min), maximum(_max) {}
			
			internal u64 rng_diff1u64(Rng1u64 rng);
		};

		// 2D
		struct Rng2f32 {
			f32 minimum;
			f32 maximum;

			Rng2f32() : minimum(0), maximum(0) {}
			Rng2f32(f32 _min, f32 _max) : minimum(_min), maximum(_max) {}
			
			internal f32 rng_diff1u64(Rng2f32 rng);
		};


		// Find a betteer place to store these!
		// Textures, Triangles, Faces
		struct Texture2F32 {
			f32 u;
			f32 v;
		};

		struct Triangle2F32 {
			Vec4 points[3];
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

	}
}


