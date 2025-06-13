
#pragma once

namespace Starlight {
	namespace Render {

		union RenderHandle {
			u64 handle[1];
		};
		
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

		// Find a betteer place to store these!
		// Textures, Triangles, Faces
		struct Texture2F32 {
			f32 u;
			f32 v;
		};

		struct Triangle2F32 {
			Vec4 points[3];
			Texture2F32 texture_coords[3];
			u32 colour;
		};

		// Faces are the surfaces formed by connecting those points.
		struct Face3S32 {
			Texture2F32 a_uv;
			Texture2F32 b_uv;
			Texture2F32 c_uv;
			u32 colour;
			s32 a;
			s32 b;
			s32 c;
		};

	}
}


#include "render/core/render_core.h"

#if OS_WINDOWS
	#include "render/software/win32/render_win32.h"
#else
	#error "This rendering backend is not supported yet!"
#endif 

