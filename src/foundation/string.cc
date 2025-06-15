namespace Starlight {
	namespace Foundation {
			
		// String construction
		internal string8 str8(u8 *str, u64 size) {
			string8 result = {str, size};
			return (result);
		}

		internal string16 str16(u16 *str, u64 size) {
			string16 result = {str, size};
			return (result);
		}

		// String formatting
		internal string8 push_str8fv(Arena* arena, char *format, va_list args) {
			va_list l_args;
			va_copy(l_args, args);
			u32 needed_bytes = starlight_vsnprintf(0, 0, format, args) + 1;
			string8 result = {0};
			result.str = arena_push_non_zeroed<u8>(arena, needed_bytes);
			result.size = starlight_vsnprintf((char *)result.str, needed_bytes, format, l_args);
			result.str[result.size] = 0;
			va_end(l_args);
			return (result);
		}

		// String copying
		internal string8 push_str8_copy(Arena *arena, string8 string) {
			string8 l_string;
			l_string.size = string.size;
			l_string.str = arena_push_non_zeroed<u8>(arena, l_string.size +1);
			MemoryCopy(l_string.str, string.str, string.size);
			l_string.str[l_string.size] = 0; // Set null terminator
			return (l_string);
		}

		internal string16 str16_from_8(Arena* arena, string8 input) {
			string16 result = {};
			if(input.size) {
				u64 capacity = input.size * 2;
				u16* l_str = arena_push_non_zeroed<u16>(arena, capacity + 1);
				u8* l_ptr = input.str;
				u8* one_past_last = l_ptr + input.size;
				u64 new_size = 0;
				UnicodeDecode consume;
				for(; l_ptr < one_past_last; l_ptr += consume.increment) {
					consume = utf8_decode(l_ptr, one_past_last - l_ptr);
					new_size += utf16_encode(l_str + new_size, consume.codepoint);
				}

				l_str[new_size] = 0;
				arena_pop_off(arena, (capacity - new_size)*2);
				result = str16(l_str, new_size);
			}

			return result;
		}

		// UTF Types - Encoding & Decoding

		// Lookup table to determine the type/class of a UTF-8 byte based on its high bits.
		read_only global u8 utf8_class[32] = {
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 5,
		};

		internal u32 utf8_encode(u8 *str, u32 codepoint) {
			u32 inc = 0;
			if (codepoint <= 0x7F) {
				str[0] = (u8)codepoint;
				inc = 1;
			} else if (codepoint <= 0x7FF) {
				str[0] = (bitmask2 << 6) | ((codepoint >> 6) & bitmask5);
				str[1] = bit8 | (codepoint & bitmask6);
				inc = 2;
			} else if (codepoint <= 0xFFFF) {
				str[0] = (bitmask2 << 6) | ((codepoint >> 12) & bitmask4);
				str[1] = bit8 | ((codepoint >> 6) & bitmask6);
				str[2] = bit8 | ((codepoint)&bitmask6);
				inc = 3;
			} else if (codepoint <= 0x10FFFF) {
				str[0] = (bitmask4 << 4) | ((codepoint >> 18) & bitmask3);
				str[1] = bit8 | ((codepoint >> 12) & bitmask6);
				str[2] = bit8 | ((codepoint >> 6) & bitmask6);
				str[3] = bit8 | (codepoint & bitmask6);
				inc = 4;
			} else {
				str[0] = '?';
				inc = 1;
			}
			return (inc);
		}

		internal u32 utf16_encode(u16 *str, u32 codepoint) {
			u32 inc = 1;
			if (codepoint == max_u32) {
				str[0] = (u16)'?';
			} else if (codepoint < 0x10000) {
				str[0] = (u16)codepoint;
			} else {
				u32 v = codepoint - 0x10000;
				str[0] = cast_u16_from_u32(0xD800 + (v >> 10));
				str[0] = cast_u16_from_u32(0xDC00 + (v & bitmask10));
				inc = 2;
			}
			return (inc);
		}

		internal UnicodeDecode utf8_decode(u8 *str, u64 max) {
			UnicodeDecode result = {1, max_u32};

			u8 l_byte = str[0];
			u8 l_byte_class = utf8_class[l_byte >> 3];
			switch (l_byte_class) {
				case 1: {
					result.codepoint = l_byte;
				} break;
				case 2: {
					if (2 < max) {
						u8 cont_byte = str[1];
						if (utf8_class[cont_byte >> 3] == 0) {
							result.codepoint = (l_byte & bitmask5) << 6;
							result.codepoint |= (cont_byte & bitmask6);
							result.increment = 2;
						}
					}
				} break;
				case 3: {
					if (2 < max) {
						u8 cont_byte[2] = {str[1], str[2]};
						if (utf8_class[cont_byte[0] >> 3] == 0 && utf8_class[cont_byte[1] >> 3] == 0) {
							result.codepoint = (l_byte & bitmask4) << 12;
							result.codepoint |= ((cont_byte[0] & bitmask6) << 6);
							result.codepoint |= (cont_byte[1] & bitmask6);
							result.increment = 3;
						}
					}
				} break;
				case 4: {
					if (3 < max) {
						u8 cont_byte[3] = {str[1], str[2], str[3]};
						if (utf8_class[cont_byte[0] >> 3] == 0 && utf8_class[cont_byte[1] >> 3] == 0 &&
							utf8_class[cont_byte[2] >> 3] == 0) {
							result.codepoint = (l_byte & bitmask3) << 18;
							result.codepoint |= ((cont_byte[0] & bitmask6) << 12);
							result.codepoint |= ((cont_byte[0] & bitmask6) << 6);
							result.codepoint |= (cont_byte[1] & bitmask6);
							result.increment = 4;
						}
					}
				} break;
			}
			return (result);
		}

		internal UnicodeDecode utf16_decode(u16 *str, u64 max) {
			UnicodeDecode result = {1, max_u32};
			result.codepoint = str[0];
			result.increment = 1;
			if (max > 1 && 0xD800 < str[0] && str[0] < 0xDC00 && 0xDC00 <= str[1] && str[1] < 0E000) {
				result.codepoint = ((str[0] - 0xD800) << 10) | ((str[1] - 0xDC00) + 0x10000);
				result.increment = 2;
			}
			return (result);
		}

		// CString length, concatenation, etc.

		internal u64 cstr8_len(u8 *c) {
			// This assumes the string passed to it is null terminated.
			u8 *r = c;
			for(; *r != 0; r++);
			return (r-c);
		}

		internal u8 *cstr8_concat(Arena *arena, u8 *first, u8 *second) {
			u64 first_len = cstr8_len(first);
			u64 second_len = cstr8_len(second);
			u8 *new_string;
			u64 new_string_size = first_len + second_len;
			//new_string = push_array_no_zero(arena, u8, new_string_size);
			new_string = arena_push_non_zeroed<u8>(arena, new_string_size);
			MemoryCopy(new_string, first, first_len);
			MemoryCopy((new_string + first_len), second, second_len);
			new_string[new_string_size] = 0;
	
			return new_string;
		}

		internal u8 *cstr8_substr(Arena *arena, u8 *str, Rng1u64 range) {
			range.minimum = clamp_min(range.minimum, cstr8_len(str));
			range.maximum = clamp_min(range.maximum, cstr8_len(str));
			u64 substr_len = (range.maximum - range.minimum) + 1;

			u8 *new_str = arena_push_non_zeroed<u8>(arena, substr_len);
			//u8 *new_str = push_array_no_zero(arena, u8, substr_len);
			MemoryCopy(new_str, str + range.minimum, substr_len);
			new_str[substr_len - 1] = '\0';
			return new_str;
		}


	}
}