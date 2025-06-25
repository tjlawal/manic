#pragma once

namespace Starlight {
	namespace ResourceManager {
		namespace Parser {
			
			enum FormatTokenType : u32 {
				FormatTokenType_Unknown,

				FormatTokenType_Grouping,
				FormatTokenType_SmootingGroup,
				FormatTokenType_ObjectName,

				FormatTokenType_GeometricVertices,
				FormatTokenType_TextureVertices,
				FormatTokenType_VertexNormals,
				FormatTokenType_Face,
				
				FormatTokenType_MaterialName,
				FormatTokenType_MaterialLibrary,
				FormatTokenType_Float,
				FormatTokenType_Int,
				FormatTokenType_Slash,
				FormatTokenType_Minus,

				FormatTokenType_Line,
				FormatTokenType_Point,
				
				FormatTokenType_EOF,
			};



		}
	}
}