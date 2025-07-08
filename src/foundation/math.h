/************************************************************************************
* 
* This interface provides math functions to work with Vector2(signed and floats), 
* Vector3, Ranges, and Matrix.
* 
* Conventions:
* 	- The Vec2s structure is mostly used to GFX windows.
* 	- The Ranges structure is used to represent the highes and lowest values. It is 
*			especially handy when trying to calculate file sizes, window rect, etc.
*		- The Matrix structure is defined as Row-Major, and all operations and 
*			parameter naming follow that structure. When dealing with Column-Major API, 
*			machinery for inversion is provided. Example: row0 is [m0 m1 m2 m3].
* 	- All functions are always self-contained, meaning one function doesn't use other
* 		functions defined by this library inside, it is directly re-implemented.
*		- All functions are always inlined.
*		- All functions input parameters are always by value!
*		- All functions use a "result " variable to return results of computations 
*			(except C++ operators).
*		- Angles are always in radians, macros are provided to convert to/from degrees.
*		- All structures are 32-bits aligned for AVX2.
* 
* Types:
*		- Vectors:
* 		- Vector2, signed and float.
* 		- Vector3
* 		- Vector4
* 	- Matrices:
*			- 4 x 4
*		Ranges:
*			- 1 Dimension
* 		- 2 Dimension
* 	
*
************************************************************************************/

#pragma once

#define MATH_PI 3.141592653589793238462643383279502884197169399375f

namespace Starlight {
	namespace Foundation {

		// --------------------------------------------------
		// Forward declaration so order of use is irrelevant.
		// --------------------------------------------------
		struct Vec2f;
		struct Vec2s;
		struct Vec3;
		struct Vec4;
		struct Matrix;
		struct Rng1u64;

		// Vertex 
		union Vertex {
			struct {
				f32 x;
				f32 y;
				f32 z;
			};

			f32 v[3];
		};

		//-----------------------------------------------
		// Vecto2 Math - seperated into signed and float
		//----------------------------------------------

		// This is mostly used to represent gfx window.
		struct Vec2s {
			s32 x;
			s32 y;

			Vec2s() : x(0), y(0) {}
			Vec2s(s32 _x, s32 _y) : x(_x), y(_y) {}
		};

		// This is used for math stuff.
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

		// ----------------------------------------
		// Vec3
		// ----------------------------------------

		struct Vec3 {
			f32 x;
			f32 y;
			f32 z;
		};

		FORCE_INLINE internal Vec3 vec3_add(Vec3 v1, Vec3 v2) {
			Vec3 result = {
				v1.x + v2.x,
				v1.y + v2.y,
				v1.z + v2.z
			};

			return result;
		}

		FORCE_INLINE internal Vec3 vec3_subtract(Vec3 v1, Vec3 v2) {
			Vec3 result = {
				v1.x - v2.x,
				v1.y - v2.y,
				v1.z - v2.z
			};

			return result;
		}

		FORCE_INLINE internal Vec3 vec3_multiply(Vec3 v1, Vec3 v2) {
			Vec3 result = {
				v1.x * v2.x,
				v1.y * v2.y,
				v1.z * v2.z
			};

			return result;
		}

		FORCE_INLINE internal Vec3 vec3_divide(Vec3 v1, Vec3 v2) {
			Vec3 result = {
				v1.x / v2.x,
				v1.y / v2.y,
				v1.z / v2.z
			};

			return result;
		}

		FORCE_INLINE internal Vec3 vec3_normalize(Vec3 v) {
			Vec3 result = {};
			f32 length = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
			
			if(length != 0.0f) {
				f32 n = 1.0f / length;
				result.x *= n;
				result.y *= n;
				result.z *= n;
			}

			return result;
		}

		FORCE_INLINE internal f32 vec3_dot_product(Vec3 v1, Vec3 v2) {
			f32 result = (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
			return result;
		}

		FORCE_INLINE internal Vec3 vec3_from_vec4(Vec4 v) {
		  Vec3 result = {v.x, v.y, v.z};
		  return result;
		}

		
		// ----------------------------------------
		// Vec3 operator overloads
		// ----------------------------------------
		FORCE_INLINE internal Vec3 operator+(const Vec3& lhs, const Vec3& rhs) { return vec3_add(lhs, rhs); }
		FORCE_INLINE internal Vec3 operator-(const Vec3& lhs, const Vec3& rhs) { return vec3_subtract(lhs, rhs); } 		
		FORCE_INLINE internal Vec3 operator*(const Vec3& lhs, const Vec3& rhs) { return vec3_multiply(lhs, rhs); } 		
		FORCE_INLINE internal Vec3 operator/(const Vec3& lhs, const Vec3& rhs) { return vec3_divide(lhs, rhs); } 		

		FORCE_INLINE internal const Vec3& operator+=(Vec3&lhs, const Vec3& rhs) {
			lhs = vec3_add(lhs, rhs);
			return lhs;
		}

		FORCE_INLINE internal const Vec3& operator-=(Vec3& lhs, const Vec3& rhs) {
			lhs = vec3_subtract(lhs, rhs);
			return lhs;
		}

		FORCE_INLINE internal const Vec3& operator*=(Vec3& lhs, const Vec3& rhs) { 
			lhs = vec3_multiply(lhs, rhs); 
			return lhs;
		} 

		FORCE_INLINE internal const Vec3& operator/=(Vec3& lhs, const Vec3& rhs) {
			lhs = vec3_divide(lhs, rhs);
			return lhs;
		}

		// ----------------------------------------
		// Vec4
		// ----------------------------------------

		struct Vec4 {
			f32 x;
			f32 y;
			f32 z;
			f32 w;
		};

		FORCE_INLINE internal Vec4 vec4_add(Vec4 v1, Vec4 v2){
			Vec4 result = {
				v1.x + v2.x,
				v1.y + v2.y,
				v1.z + v2.z,
				v1.w + v2.w
			};
			
			return result;
		}

		FORCE_INLINE internal Vec4 vec4_subtract(Vec4 v1, Vec4 v2) {
			Vec4 result = {
				v1.x - v2.x,
				v1.y - v2.y,
				v1.z - v2.z,
				v1.w - v2.w
			};
			
			return result;
		}

		FORCE_INLINE internal Vec4 vec4_multiply(Vec4 v1, Vec4 v2) {
			Vec4 result = {
				v1.x * v2.x,
				v1.y * v2.y,
				v1.z * v2.z,
				v1.w * v2.w
			};

			return result;
		}

		FORCE_INLINE internal Vec4 vec4_divide(Vec4 v1, Vec4 v2) {
			Vec4 result = {
				v1.x / v2.x,
				v1.y / v2.y,
				v1.z / v2.z,
				v1.w / v2.w
			};

			return result;
		}

		FORCE_INLINE internal Vec4 vec4_normalize(Vec4 v) {
			Vec4 result = {};
			f32 length = sqrtf((v.x*v.x) + (v.y*v.y) + (v.z*v.z) + (v.w*v.w));

			if (length > 0) {
				f32 n = 1.0f/length;
				result.x = v.x * n;
				result.y = v.y * n;
				result.z = v.z * n;
				result.w = v.w * n;
			}

			return result;
		}

		FORCE_INLINE internal Vec4 vec4_scale(Vec4 v, f32 scale) {
			Vec4 result = {
				v.x * scale,
				v.y * scale,
				v.z * scale,
				v.w * scale
			};

			return result;
		}

		FORCE_INLINE internal Vec4 vec4_from_vec3(Vec3 v) {
			Vec4 result = {v.x, v.y, v.z, 1.0};
			return result;
		}

		// ----------------------------------------
		// Vec4 operator overloads
		// ----------------------------------------
		FORCE_INLINE internal Vec4 operator+(const Vec4& lhs, const Vec4& rhs) { return vec4_add(lhs, rhs); }
		FORCE_INLINE internal Vec4 operator-(const Vec4& lhs, const Vec4& rhs) { return vec4_subtract(lhs, rhs); } 		
		FORCE_INLINE internal Vec4 operator*(const Vec4& lhs, const Vec4& rhs) { return vec4_multiply(lhs, rhs); } 		
		FORCE_INLINE internal Vec4 operator/(const Vec4& lhs, const Vec4& rhs) { return vec4_divide(lhs, rhs); } 		

		FORCE_INLINE internal const Vec4 operator+=(Vec4&lhs, const Vec4& rhs) {
			lhs = vec4_add(lhs, rhs);
			return lhs;
		}

		FORCE_INLINE internal const Vec4& operator-=(Vec4& lhs, const Vec4& rhs) {
			lhs = vec4_subtract(lhs, rhs);
			return lhs;
		}
		
		FORCE_INLINE internal const Vec4& operator/=(Vec4& lhs, const Vec4& rhs) {
			lhs = vec4_divide(lhs, rhs);
			return lhs;
		}

		FORCE_INLINE internal const Vec4& operator*=(Vec4& lhs, const Vec4& rhs) { 
			lhs = vec4_multiply(lhs, rhs); 
			return lhs;
		} 


		// ----------------------------------------
		// Common vector operations
		// ----------------------------------------

		//FORCE_INLINE internal f32 dot(const Vec2f& a, const Vec2f& b) { 
		//	return ((a.x * b.x) + (a.y * b.y)); 
		//}
		
		//FORCE_INLINE internal f32 dot(const Vec3& a, const Vec3& b) { 
		//	return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z)); 
		//}

		//FORCE_INLINE internal f32 dot(const Vec4& a, const Vec4& b) { 
		//	return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w)); 
		//}

		//FORCE_INLINE internal void normalize(Vec2f* v) {
		//	f32 length = sqrtf((v->x * v->x) + (v->y * v->y));

		//	v->x /= length;
		//	v->y /= length;
		//}

		//FORCE_INLINE internal void normalize(Vec3* v) {
		//	f32 length = sqrtf((v->x * v->x) + (v->y * v->y) + (v->z * v->z));

		//	v->x /= length;
		//	v->y /= length;
		//	v->z /= length;
		//}

		//FORCE_INLINE internal void normalize(Vec4* v) {
		//	f32 length = sqrtf((v->x * v->x) + (v->y * v->y) + (v->z * v->z) + (v->w * v->w));

		//	v->x /= length;
		//	v->y /= length;
		//	v->z /= length;
		//	v->w /= length;
		//}

		//FORCE_INLINE internal Vec3 cross(const Vec3& a, const Vec3& b) {
		//	return Vec3 { (a.y * b.z) - (a.z * b.y), (a.z * b.x) - (a.x * b.z), (a.x * b.y) - (a.y * b.x) };
		//}

		
		// ----------------------------------------
		// Matrix - 4 x 4
		// ----------------------------------------
		struct Matrix {
			f32  m0,  m1,  m2,  m3; // Row 1
			f32  m4,  m5,  m6,  m7; // Row 2
			f32  m8,  m9, m10, m11; // Row 3
			f32 m12, m13, m14, m15; // Row 4
		};

		// @REVISE: Could this be even more faster?

		FORCE_INLINE internal Matrix matrix_identity(void) {
			Matrix result = { 
				1.0f, 0.0f, 0.0f, 0.0f, 
				0.0f, 1.0f, 0.0f, 0.0f, 
				0.0f, 0.0f, 1.0f, 0.0f, 
				0.0f, 0.0f, 0.0f, 1.0f
			};

			return result;
		}

		FORCE_INLINE internal Matrix matrix_add(Matrix first, Matrix second) {
			Matrix result = {};

			result.m0  = first.m0 + second.m0;
			result.m1  = first.m1 + second.m1;
			result.m2  = first.m2 + second.m2;
			result.m3  = first.m3 + second.m3;
			result.m4  = first.m4 + second.m4;
			result.m5  = first.m5 + second.m5;
			result.m6  = first.m6 + second.m6;
			result.m7  = first.m7 + second.m7;
			result.m8  = first.m8 + second.m8;
			result.m9  = first.m9 + second.m9;
			result.m10 = first.m10 + second.m10;
			result.m11 = first.m11 + second.m11;
			result.m12 = first.m12 + second.m12;
			result.m13 = first.m13 + second.m13;
			result.m14 = first.m14 + second.m14;
			result.m15 = first.m15 + second.m15;

			return result;
		}

		FORCE_INLINE internal Matrix matrix_subtract(Matrix first, Matrix second) {
			Matrix result = {};

			result.m0  = first.m0 - second.m0;
			result.m1  = first.m1 - second.m1;
			result.m2  = first.m2 - second.m2;
			result.m3  = first.m3 - second.m3;
			result.m4  = first.m4 - second.m4;
			result.m5  = first.m5 - second.m5;
			result.m6  = first.m6 - second.m6;
			result.m7  = first.m7 - second.m7;
			result.m8  = first.m8 - second.m8;
			result.m9  = first.m9 - second.m9;
			result.m10 = first.m10 - second.m10;
			result.m11 = first.m11 - second.m11;
			result.m12 = first.m12 - second.m12;
			result.m13 = first.m13 - second.m13;
			result.m14 = first.m14 - second.m14;
			result.m15 = first.m15 - second.m15;

			return result;
		}

		// Matrix multiplication does not care about how it is represented in memory (row-major or column-major)!
		FORCE_INLINE internal Matrix matrix_multiply(Matrix first, Matrix second) {
			Matrix result = {};

			result.m0 = (first.m0 * second.m0) + (first.m1 * second.m4) + (first.m2 * second.m8)  + (first.m3 * second.m12);
			result.m1 = (first.m0 * second.m1) + (first.m1 * second.m5) + (first.m2 * second.m9)  + (first.m3 * second.m13);
			result.m2 = (first.m0 * second.m2) + (first.m1 * second.m6) + (first.m2 * second.m10) + (first.m3 * second.m14);
			result.m3 = (first.m0 * second.m3) + (first.m1 * second.m7) + (first.m2 * second.m11) + (first.m3 * second.m15);

			result.m4 = (first.m4 * second.m0) + (first.m5 * second.m4) + (first.m6 * second.m8)  + (first.m7 * second.m12);
			result.m5 = (first.m4 * second.m1) + (first.m5 * second.m5) + (first.m6 * second.m9)  + (first.m7 * second.m13);
			result.m6 = (first.m4 * second.m2) + (first.m5 * second.m6) + (first.m6 * second.m10) + (first.m7 * second.m14);
			result.m7 = (first.m4 * second.m3) + (first.m5 * second.m7) + (first.m6 * second.m11) + (first.m7 * second.m15);

			result.m8  = (first.m8 * second.m0) + (first.m9 * second.m4) + (first.m10 * second.m8)  + (first.m11 * second.m12);
			result.m9  = (first.m8 * second.m1) + (first.m9 * second.m5) + (first.m10 * second.m9)  + (first.m11 * second.m13);
			result.m10 = (first.m8 * second.m2) + (first.m9 * second.m6) + (first.m10 * second.m10) + (first.m11 * second.m14);
			result.m11 = (first.m8 * second.m3) + (first.m9 * second.m7) + (first.m10 * second.m11) + (first.m11 * second.m15);

			result.m12 = (first.m12 * second.m0) + (first.m13 * second.m4) + (first.m14 * second.m8)  + (first.m15 * second.m12);
			result.m13 = (first.m12 * second.m1) + (first.m13 * second.m5) + (first.m14 * second.m9)  + (first.m15 * second.m13);
			result.m14 = (first.m12 * second.m2) + (first.m13 * second.m6) + (first.m14 * second.m10) + (first.m15 * second.m14);
			result.m15 = (first.m12 * second.m3) + (first.m13 * second.m7) + (first.m14 * second.m11) + (first.m15 * second.m15);

			return result;
		}

		FORCE_INLINE internal Matrix matrix_transpose(Matrix m) {};

		FORCE_INLINE internal Matrix matrix_rotate_x(f32 angle) {
			Matrix result = { 
				1.0f, 0.0f, 0.0f, 0.0f, 
				0.0f, 1.0f, 0.0f, 0.0f, 
				0.0f, 0.0f, 1.0f, 0.0f, 
				0.0f, 0.0f, 0.0f, 1.0f
			};

			f32 cos_result = cosf(angle);
			f32 sin_result = sinf(angle);

			result.m5 = cos_result;
			result.m6 = -sin_result;
			result.m9 = sin_result;
			result.m10 = cos_result;

			return result;
		}

		FORCE_INLINE internal Matrix matrix_rotate_y(f32 angle) {
			Matrix result = { 
				1.0f, 0.0f, 0.0f, 0.0f, 
				0.0f, 1.0f, 0.0f, 0.0f, 
				0.0f, 0.0f, 1.0f, 0.0f, 
				0.0f, 0.0f, 0.0f, 1.0f
			};

			f32 cos_result = cosf(angle);
			f32 sin_result = sinf(angle);

			result.m0 = cos_result;
			result.m2 = sin_result;
			result.m8 = -sin_result;
			result.m10 = cos_result;

			return result;
		}

		FORCE_INLINE internal Matrix matrix_rotate_z(f32 angle) {
			Matrix result = { 
				1.0f, 0.0f, 0.0f, 0.0f, 
				0.0f, 1.0f, 0.0f, 0.0f, 
				0.0f, 0.0f, 1.0f, 0.0f, 
				0.0f, 0.0f, 0.0f, 1.0f
			};

			f32 cos_result = cosf(angle);
			f32 sin_result = sinf(angle);

			result.m0 = cos_result;
			result.m1 = sin_result;
			result.m4 = -sin_result;
			result.m5 = cos_result;

			return result;
		}

		FORCE_INLINE internal Matrix matrix_scale(f32 x, f32 y, f32 z) {
			Matrix result = {
				x, 0.0f, 0.0f, 0.0f,
				0.0f, y, 0.0f, 0.0f, 
				0.0f, 0.0f, z, 0.0f, 
				0.0f, 0.0f, 0.0f, 1.0f
			};

			result.m0 = x;
			result.m5 = y;
			result.m10 = z;

			return result;
		}

		FORCE_INLINE internal Matrix matrix_translate(f32 x, f32 y, f32 z){
			Matrix result = { 
				1.0f, 0.0f, 0.0f, 0.0f, 
				0.0f, 1.0f, 0.0f, 0.0f, 
				0.0f, 0.0f, 1.0f, 0.0f, 
				0.0f, 0.0f, 0.0f, 1.0f
			};

			result.m3 = x;
			result.m7 = y;
			result.m11 = z;

			return result;
		}

		FORCE_INLINE internal Vec4 matrix_multiply_vec4(Matrix m, Vec4 v) {
			Vec4 result;
			
			result.x = (m.m0 * v.x)  + (m.m1 * v.y)  + (m.m2 * v.z)  + (m.m3 * v.w);
			result.y = (m.m4 * v.x)  + (m.m5 * v.y)  + (m.m6 * v.z)  + (m.m7 * v.w);
			result.z = (m.m8 * v.x)  + (m.m9 * v.y)  + (m.m10 * v.z) + (m.m11 * v.w);
			result.w = (m.m12 * v.x) + (m.m13 * v.y) + (m.m14 * v.z) + (m.m15 * v.w);

			return result;
		}

		FORCE_INLINE Matrix perspective(f32 fov, f32 aspect, f32 znear, f32 zfar) {
			Matrix result = {};
			
			f32 f = 1 / (tanf(fov / 2));
			f32 z_normalize = zfar / (zfar - znear);

			result.m0 = aspect * f;
			result.m5 = f;
			result.m10 = z_normalize;
			result.m11 = -z_normalize * znear;
			result.m14 = 1.0f;

			return result;
		}

		// @REVISE!
		FORCE_INLINE internal Vec4 mat4f32_mul_projection(Matrix m, Vec4 v) {
			Vec4 result;
			result.x = (m.m0 * v.x)  + (m.m1 * v.y)  + (m.m2 * v.z)  + (m.m3 * v.w);
			result.y = (m.m4 * v.x)  + (m.m5 * v.y)  + (m.m6 * v.z)  + (m.m7 * v.w);
			result.z = (m.m8 * v.x)  + (m.m9 * v.y)  + (m.m10 * v.z) + (m.m11 * v.w);
			result.w = (m.m12 * v.x) + (m.m13 * v.y) + (m.m14 * v.z) + (m.m15 * v.w);

			// Perform perspective divide with original z-value that is stored in 
			// the projection matrix 'w', hence normalizing the entire image space.
			if (result.w != 0.0f) {
				result.x /= result.w;
				result.y /= result.w;
				result.z /= result.w;
			}
			return result;	
		}

		// ----------------------------------------
		// Matrix operator overloads
		// ----------------------------------------
		FORCE_INLINE Matrix operator+(const Matrix& lhs, const Matrix& rhs) { return matrix_add(lhs, rhs); }
		FORCE_INLINE Matrix operator-(const Matrix& lhs, const Matrix& rhs) { return matrix_subtract(lhs, rhs); }
		FORCE_INLINE Matrix operator*(const Matrix& lhs, const Matrix& rhs) { return matrix_multiply(lhs, rhs); }


		// Ranges

		// 1D
		struct Rng1u64 {
			u64 minimum;
			u64 maximum;

			Rng1u64() : minimum(0), maximum(0) {}
			Rng1u64(u64 _min, u64 _max) : minimum(_min), maximum(_max) {}
			
			internal u64 sizeof_rng1u(Rng1u64 rng);
		};

		// 2D
		union Rng2f32 {
			struct {
				Vec2f p0;
				Vec2f p1;
			};

			struct {
				Vec2f minimum;
				Vec2f maximum;
			};

			struct {
				f32 x0;
				f32 y0;
				f32 x1;
				f32 y1;
			};

			Vec2f v[2];

			// Constructor
			Rng2f32() {}
			Rng2f32(Vec2f _p0, Vec2f _p1) : p0(_p0), p1(_p1) {}
			Rng2f32(f32 _x0, f32 _y0, f32 _x1, f32 _y1) : x0(_x0), y0(_y0), x1(_x1), y1(_y1) {}

			internal Vec2f dim2f32(Rng2f32 r);
		};
	}
}


