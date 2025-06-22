#pragma once

using namespace Starlight::Foundation;
using namespace Starlight::Platform;
using namespace Starlight::Render;

namespace Starlight {
	
	struct GameState {
		Arena* game_memory; 			// Persistent data goes here
		Arena* per_frame_memory;  // Per-frame data, cleared each frame

		Renderer render_buffer;
		Handle os_handle;
		Rng2f32	window_dim;
	};

	//RenderHandle render_handle;
	//struct GameState {};
}