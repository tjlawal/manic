
#pragma once

namespace Starlight {
	namespace ResourceManager {
		namespace Parser {
			
			// Parser token types, just add future file formats here, it shouldn't be a problem, I hope. Tijani - 06/08/2025
			enum ParserTokenType : u32 {
				ParserTokenType_Unknown,

				// Wavefront Obj token types
				ParserTokenType_Grouping,
				ParserTokenType_SmootingGroup,
				ParserTokenType_ObjectName,

				ParserTokenType_Line, 
				ParserTokenType_Point,
				ParserTokenType_Face,

				ParserTokenType_GeometricVertices,
				ParserTokenType_TextureVertices,
				ParserTokenType_VertexNormals,
	
				ParserTokenType_MaterialName,
				ParserTokenType_MaterialLibrary,

				ParserTokenType_Float,
				ParserTokenType_Int,

				ParserTokenType_Slash,
				ParserTokenType_Minus,

				ParserTokenType_EOF,
			};



		}
	}
}