
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

		internal DateTime date_time_from_dense_time(DenseTime time) {}

		internal DenseTime dense_time_from_date_time(DateTime time) {
			DenseTime result = {};
			result += time.year;
			result *= 12;
			
			result += time.mon;
			result *= 31;
			
			result += time.day;
			result *= 24;

			result += time.hour;
			result *= 60;

			result += time.minute;
			result *= 61; // Accounting for leap-second

			result += time.second;
			result *= 1000;

			result += time.milli_second;
			return result;
		}

	}


}