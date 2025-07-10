using namespace Starlight::Foundation;

namespace Starlight {
	namespace Foundation {

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
