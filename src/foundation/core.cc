
namespace Starlight {
	namespace Foundation {
	
		u16 cast_u16_from_u32(u32 x) {
			AssertAlways(x <= max_u16);
			u16 result = static_cast<u16>(x);
			return result;
		}

		u32 saturate_u32_from_u64(u64 x) {
			u32 y = (x > max_u32) ? max_u32 : static_cast<u32>(x);
			return y;
		}
	}


}