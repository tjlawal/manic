#pragma once

using namespace Starlight::Foundation;
using namespace Starlight::Platform;
using namespace Starlight::Render;

namespace Starlight {
	
	struct GameWindowState {
		Arena* 			 arena;
		Handle 			 os_handle;
		RenderHandle render_handle;
		Rng2f32 		 window_rect;
	};

	struct GameState {
		Arena* per_frame_memory;
	};
}