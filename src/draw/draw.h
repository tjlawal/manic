
#pragma once

using namespace Starlight::Foundation;
using namespace Starlight::Render;

namespace Starlight {
	namespace Draw {

		internal void draw_grid(Renderer *buffer, s32 width, s32 height);
		internal void draw_rect(Renderer *buffer, s32 x, s32 y, s32 width, s32 height, u32 colour);
		internal void draw_pixel(Renderer *buffer, s32 x, s32 y, u32 colour);
		internal void draw_line_dda(Renderer *buffer, s32 x0, s32 y0, s32 x1, s32 y1, u32 colour);
		internal void draw_triangle(Renderer *buffer, s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2, u32 colour);
		internal void draw_triangle_pixel(Renderer *buffer, s32 x, s32 y, Vec4 point_a, Vec4 point_b, Vec4 point_c,  u32 colour);
		internal void draw_filled_triangle(Renderer *buffer,  s32 x0, s32 y0, f32 z0, f32 w0, s32 x1, s32 y1, f32 z1, f32 w1, s32 x2, s32 y2, f32 z2, f32 w2, u32 colour);
		internal void draw_texel(Renderer *buffer, s32 x, s32 y, Vec4 point_a, Vec4 point_b, Vec4 point_c,  TextureCoord a_uv, TextureCoord b_uv, TextureCoord c_uv,  u32 *texture, s32 texture_width, s32 texture_height);
		internal void draw_textured_triangle(Renderer *buffer,  s32 x0, s32 y0, f32 z0, f32 w0, f32 u0, f32 v0,  s32 x1, s32 y1, f32 z1, f32 w1, f32 u1, f32 v1,  s32 x2, s32 y2, f32 z2, f32 w2, f32 u2, f32 v2,  u32 *texture, s32 texture_width, s32 texture_height);
	}
}