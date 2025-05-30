using namespace Starlight::Foundation;

namespace Starlight {
	namespace Foundation {

		// Vectors
		// 2D - Floating Point
		f32 Vec2F32::dot(Vec2F32 a, Vec2F32 b) { return ((a.x * b.x) + (a.y * b.y)); }
		f32 Vec2F32::length(Vec2F32 vector) { return f32(vector.x * vector.x + vector.y * vector.y); }

		// 2D - Signed
		f32 Vec2S32::dot(Vec2F32 a, Vec2F32 b) { return ((a.x * b.x) + (a.y * b.y)); }
		f32 Vec2S32::length(Vec2F32 vector) { return s32(vector.x * vector.x + vector.y * vector.y); }

		Vec2F32 Vec2F32::vec2f32_from_vec4f32(Vec4F32 vector) { return Vec2F32 {vector.x, vector.y}; }

		// 3D
		f32 Vec3F32::dot(Vec3F32 a, Vec3F32 b) { return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z)); }
		f32 Vec3F32::length(Vec3F32 v) { return (f32)sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }
		
		void Vec3F32::normalize(Vec3F32* v) {
			f32 length = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);

			v->x /= length;
			v->y /= length;
			v->z /= length;
		}

		Vec3F32 Vec3F32::cross(Vec3F32 a, Vec3F32 b) {
			return Vec3F32 {
				(a.y * b.z) - (a.z * b.y), 
				(a.z * b.x) - (a.x * b.z), 
				(a.x * b.y) - (a.y * b.x)
			};
		}

		Vec3F32 vec3f32_rotate_x(Vec3F32 vector, f32 new_angle) {
		  return Vec3F32 {
				vector.x,
				vector.y * (f32)cos(new_angle) - vector.z * (f32)sin(new_angle),
				vector.y * (f32)sin(new_angle) + vector.z * (f32)cos(new_angle)
			};
		}

		Vec3F32 vec3f32_rotate_y(Vec3F32 vector, f32 new_angle) {
		  return Vec3F32 {
				vector.x * (f32)cos(new_angle) - vector.z * (f32)sin(new_angle),
				vector.y,
				vector.x * (f32)sin(new_angle) + vector.z * (f32)cos(new_angle)
			};
		}

		Vec3F32 vec3f32_rotate_z(Vec3F32 vector, f32 new_angle) {
		  return Vec3F32 {
				vector.x * (f32)cos(new_angle) - vector.y * (f32)sin(new_angle),
				vector.x * (f32)sin(new_angle) + vector.y * (f32)cos(new_angle),
				vector.z
			};
		}

		Vec3F32 Vec3F32::vec3f32_from_vec4f32(Vec4F32 v) {
		  Vec3F32 result = {v.x, v.y, v.z};
		  return result;
		}

		// 4D
		Vec4F32 Vec4F32::vec4f32_from_vec3f32(Vec3F32 v) {
			Vec4F32 result = {v.x, v.y, v.z, 1.0};
			return result;
		}

		// Ranges
		// 1-dimension
		Rng1u64 rng1u64(u64 min, u64 max) {
			Rng1u64 r = {min, max};
			if(r.minimum > r.maximum) 
				swap(r.minimum, r.maximum);
			return r;
		}

		u64 rng_diff1u64(Rng1u64 rng) {
			u64 r = rng.maximum - rng.minimum;
			return r;
		}

	}
}
