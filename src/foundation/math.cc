using namespace Starlight::Foundation;

namespace Starlight {
	namespace Foundation {

		Vec2f Vec2f::vec2f_from_vec4(Vec4 v) { return Vec2f {v.x, v.y }; }
		
		//Vec3 Vec3::rotate_x(Vec3 vector, f32 new_angle) {
		//  return Vec3 {
		//		vector.x,
		//		vector.y * (f32)cos(new_angle) - vector.z * (f32)sin(new_angle),
		//		vector.y * (f32)sin(new_angle) + vector.z * (f32)cos(new_angle)
		//	};
		//}

		//Vec3 Vec3::rotate_y(Vec3 vector, f32 new_angle) {
		//  return Vec3 {
		//		vector.x * (f32)cos(new_angle) - vector.z * (f32)sin(new_angle),
		//		vector.y,
		//		vector.x * (f32)sin(new_angle) + vector.z * (f32)cos(new_angle)
		//	};
		//}

		//Vec3 Vec3::rotate_z(Vec3 vector, f32 new_angle) {
		//  return Vec3 {
		//		vector.x * (f32)cos(new_angle) - vector.y * (f32)sin(new_angle),
		//		vector.x * (f32)sin(new_angle) + vector.y * (f32)cos(new_angle),
		//		vector.z
		//	};
		//}

		// Ranges
		// 1-dimension
		Rng1u64 rng1u64(u64 min, u64 max) {
			Rng1u64 r = {min, max};
			if(r.minimum > r.maximum) 
				swap(r.minimum, r.maximum);
			return r;
		}

		u64 sizeof_rng1u(Rng1u64 rng) {
			u64 r = ((rng.maximum > rng.minimum) ? (rng.maximum - rng.minimum) : 0);
			return r;
		}

		Vec2f dim2f32(Rng2f32 rng) {
			Vec2f dim = {
				((rng.maximum.x > rng.minimum.x) ? (rng.maximum.x - rng.minimum.x) : 0),
				((rng.maximum.y > rng.minimum.y) ? (rng.maximum.y - rng.minimum.y) : 0),
			};
			return dim;
		}

	}
}
