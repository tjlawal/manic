
namespace Starlight {
	namespace ResourceManager {
		namespace Parser {

			internal void read_char(Lexer *lexer) {
				if(lexer->current_position >= lexer->input.size) {
					lexer->current_char = '\0';
				} else {
					lexer->previous_position = lexer->current_position;
					lexer->current_position = lexer->future_position;
					lexer->future_position += 1;

					lexer->previous_char = lexer->input.str[lexer->previous_position];
					lexer->current_char  = lexer->input.str[lexer->current_position];
					lexer->future_char   = lexer->input.str[lexer->future_position];
				}
			}

			internal u8 peek_ahead(Lexer *lexer) {
				if(lexer->future_position >= lexer->input.size){
					return '\0';
				} 
				return lexer->input.str[lexer->future_position];
			}

			internal void eat_all_whitespace(Lexer *lexer) {
				// Keep going until the next char is not a space
				while(lexer->current_char == ' '  || lexer->current_char == '\t' || 
					lexer->current_char == '\r' || lexer->current_char == '\n') {
					read_char(lexer);
				}
			}

			internal void eat_comments(Lexer *lexer) {
				if(lexer->current_char == '#') {
					while(!is_eol(lexer->current_char)) {
						read_char(lexer);
					}
				}
			}

			inline b8 is_digit(u8 c) {
				b8 r = ((c >= '0') && (c <= '9'));
				return r;
			}

			inline b8 is_whitespace(u8 c) {
				b8 r = ((c == ' ') || (c == '\t'));
				return r;
			}

			inline b8 is_eol(u8 c) {
				b8 r = ((c == '\n') || (c == '\r'));
				return r;
			}

			inline b8 is_alpha(u8 c) {
				b8 r = (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')));
				return r;
			}

			inline b8 is_eof(u8 c) {
				b8 r = (c == '\0');
				return r;
			}		
		}
	}
}