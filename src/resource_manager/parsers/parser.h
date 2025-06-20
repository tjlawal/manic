#pragma once

#include "token_types.inl"

using namespace Starlight::ResourceManager::Mesh;

namespace Starlight {
	namespace ResourceManager {
		namespace Parser {

			#define MAX_NUMERIC_BUFFER_SIZE 128 // Maximum number of floating point number precision.

			struct Token {
				u8* literal;
				FormatTokenType type;
			};

			struct Lexer {
				string8 input;
				u32 current_position; // current char position in the input stream
				u32 future_position;  // next char position in the input stream
				u8 current_char;
				u8 padding[3]; 				// space for future improvements

				Lexer() : input(), current_position(), future_position(), current_char() {}
				Lexer(string8 _input, u8 _current_char = ' ', u32 _current_pos = 0, u32 _future_pos = 0)  : 
				input(_input), 
				current_position(_current_pos), 
				future_position(_future_pos),
				current_char(_current_char) {}
			};

			// ResourceManager interface
			internal MeshInfo* rm_parse_data_from_file(Arena* arena, string8 data);

			// Main parsing functions
			internal Token 	next_token(Arena* arena, Lexer* lexer);
			internal void 	read_char (Lexer* lexer);
			internal u8 		peek_ahead(Lexer* lexer);

			// Helpers
			internal void 	eat_comments(Lexer* lexer);
			internal void 	eat_all_whitespace(Lexer* lexer);
			FORCE_INLINE b8 is_digit(u8 c);
			FORCE_INLINE b8 is_whitespace(u8 c);
			FORCE_INLINE b8 is_eol(u8 c);
			FORCE_INLINE b8 is_alpha(u8 c);
			FORCE_INLINE b8 is_eof(u8 c);

			internal void parse_float(Lexer* lexer, f32* output_value); // ??
		}
	}
}
