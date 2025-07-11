#pragma once

using namespace Starlight::Foundation;
using namespace Starlight::Platform;
using namespace Starlight::Render;

namespace Starlight {
	
	struct GameState {
		Arena* game_memory; 			// Persistent data goes here
		Arena* per_frame_memory;  // Per-frame data, cleared each frame
		Arena* asset_memory;

		Renderer render_buffer;
		Handle os_handle;
		Rng2f32	window_dim;

		f32 frame_dt;
	};

	// @NOTE: This is temporary, pending when I can think of a better way to architect this.
	struct Camera {
		Matrix projection;
		Vec3 position;
		f32 fov;
		f32 znear;
		f32 zfar;
		f32 aspect_ratio;
	};

	struct Light {
		Vec3 direction;
	};

	internal u32 light_intensity(u32 colour, f32 percentage) {
		percentage = clamp(percentage, 0.0f, 1.0f);
		u32 alpha = (colour & 0xFF000000);
		u32 red   = (colour & 0x00FF0000) * percentage;
		u32 green = (colour & 0x0000FF00) * percentage;
		u32 blue  = (colour & 0x000000FF) * percentage;
		u32 new_colour = alpha | (red & 0x00FF0000) | (green & 0x0000FF00) | (blue & 0x000000FF);
		return new_colour;
	}

}