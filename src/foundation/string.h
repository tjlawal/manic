#pragma once

namespace Starlight {
	namespace Foundation {

		/////////////////////////////////////////////////////////////
		// String8, used to represent everything string related.
		// This is used everywhere, especially in place of char * unless necessary.
		struct string8 {
			u8 *str;
			u64 size;

			string8() : str(nullptr), size(0) {}
			string8(u8* string, u64 str_size) : str(string), size(str_size) {}
			string8(const char* string, u64 str_size) : str(reinterpret_cast<u8*>(const_cast<char*>(string))), size(str_size) {}
		};

		// String8 Operations
		#define str8_lit(string) string8(string, (sizeof(string) - 1))

		internal string8 str8_concat(Arena* arena, string8 str1, string8 str2);
		internal string8 str8_copy(Arena* arena, string8 src);
		internal string8 str8_format_va(Arena* arena, char* format, va_list args);
		internal string8 str8_format(Arena* arena, char* format, ...);
		internal string8 push_str8_copy(Arena* arena, string8 string);

		////////////////////////////////////////////////////////////////
		// String16, String8 is converted to this for OS that use UTF-16.
		// Mostly used for unicode stuff.
		struct string16 {
			u16 *str;
			u64 size;

			string16() : str(nullptr), size(0) {}
			string16(void *string, u64 str_size) : str(reinterpret_cast<u16*>(string)), size(str_size) {}
		};


		// UTF Types - Encoding & Decoding
		struct UnicodeDecode {
			u32 increment;
			u32 codepoint;
		};

		internal u32 utf8_encode(u8 *str, u32 codepoint);
		internal u32 utf16_encode(u16 *str, u32 codepoint);
		internal UnicodeDecode utf8_decode(u8 *str, u64 max);
		internal UnicodeDecode utf18_decode(u16 *str, u64 max);
		internal string16 str16_from_8(Arena* arena, string8 input);
	}
}

