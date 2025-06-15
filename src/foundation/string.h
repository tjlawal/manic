#pragma once

namespace Starlight {
	namespace Foundation {

		struct string8 {
			u8 *str;
			u64 size;
		};

		internal string8 str8(u8 *str, u64 size);
		#define str8_lit(string)      str8((u8 *)(string), sizeof(string) - 1)

		//#define str8_lit(string)      str8((u8 *)(string), sizeof(string) - 1)
		//#define str8_lit_comp(string) {(u8 *)(string), sizeof(string) - 1}

		// Formatting, copying, etc.
		internal string8 push_str8fv(Arena* arena, char* format, va_list args);
		internal string8 push_str8_copy(Arena* arena, string8 string);



		struct string16 {
			u16 *str;
			u64 size;
		};

		internal string16 str16(u16 *str, u64 size);

		// UTF Types - Encoding & Decoding
		struct UnicodeDecode {
			u32 increment;
			u32 codepoint;
		};

		internal u32 utf8_encode(u8 *str, u32 codepoint);
		internal u32 utf16_encode(u16 *str, u32 codepoint);
		internal UnicodeDecode utf8_decode(u8 *str, u64 max);
		internal UnicodeDecode utf18_decode(u16 *str, u64 max);

		// Unicode string conversion
		//internal string16 str16_from_8(Arena *arena, string8 input);
		internal string16 str16_from_8(Arena* arena, string8 input);


		// CString construction length, concatenation, etc.
		internal u64 cstr8_len(u8* c);
		internal u8* cstr8(u8* str, u64 size);
		internal u8* cstr8_concat(Arena *arena, u8* first, u8* second);
		internal u8* cstr8_substr(Arena *arena, u8* str, Rng1u64 range);

	}
}

