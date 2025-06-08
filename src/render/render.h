
#pragma once

namespace Starlight {
	namespace Render {
		
		union Vertex {
			struct {
				f32 x;
				f32 y;
				f32 z;
			};
			f32 v[3];
		};

		struct TextureCoords {
			f32 u;
			f32 v;
			f32 w; // Optional
		};

		struct Normals {
			f32 i;
			f32 j;
			f32 k;
		};

		struct Face{
			s32 vertex_idx;
			s32 texture_idx;
			s32 normal_idx;
		};

	}
}


#include "render/core/render_core.h"

#if OS_WINDOWS
	#include "render/software/win32/render_win32.h"
#else
	#error "This rendering backend is not supported yet!"
#endif 

