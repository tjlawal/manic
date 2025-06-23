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
				u32 previous_position; // previous char position
				u32 current_position;  // current char position 
				u32 future_position;   // next char position

				u8 previous_char;
				u8 current_char;
				u8 future_char;

				Lexer() : input(), previous_position(), current_position(), 
									future_position(), previous_char(), current_char(), future_char() {}

				Lexer(string8 _input, 
							u32 _previous_pos = 0, u32 _current_pos = 0, u32 _future_pos = 0, 
							u8 _previous_char = ' ', u8 _current_char = ' ', u8 _future_char = ' ')  : 
				input(_input),
				previous_position(_previous_pos),
				current_position(_current_pos), 
				future_position(_future_pos),
				previous_char(_previous_char),
				current_char(_current_char),
				future_char(_future_char) {}
			};

			// ResourceManager interface
			internal MeshInfo* rm_parse_data_from_file(Arena* arena, string8 data);

			// Main parsing functions
			internal Token 	next_token(Lexer* lexer);
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
