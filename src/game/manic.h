#pragma once

using namespace Starlight::Foundation;
using namespace Starlight::Platform;
using namespace Starlight::Render;

namespace Starlight {
	
	struct GameState {
		Arena*   arena; // Do I actually need this??
		Arena* 	 per_frame_memory;
		Renderer render_buffer;
		Handle 	 os_handle;
		Rng2f32  window_rect;
	};

	//RenderHandle render_handle;
	//struct GameState {};
}