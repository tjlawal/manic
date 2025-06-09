#pragma once

#include "token_types.inl"

namespace Starlight {
	namespace ResourceManager {
		namespace Parser {
			
			struct Token {
				ParserTokenType type;
				u8* literal;
			};

			struct Lexer {
				string8 input;
				u8 current_char;
				u32 current_position; // current char position in the input stream
				u32 future_position;  // next char position in the input stream
			};

			internal Token 	get_token(Lexer* lexer);
			internal void 	read_char (Lexer* lexer);
			internal u8 		peek_ahead(Lexer* lexer);

			// Helpers
			FORCE_INLINE b8 is_digit(u8 c);
			FORCE_INLINE b8 is_whitespace(u8 c);
			FORCE_INLINE b8 is_eol(u8 c);
			FORCE_INLINE b8 is_alpha(u8 c);
			FORCE_INLINE b8 is_eof(u8 c);

			internal void eat_all_whitespace(Lexer* lexer);
			internal void eat_comments(Lexer* lexer);
			internal void parse_float(Lexer* lexer, f32* output_value);




		}
	}
}
