// Copyright Frost Gorilla, Inc. All Rights Reserved.

#ifndef DRAW_H
#define DRAW_H

internal void draw_pixel(RenderBackBuffer *buffer, s32 x, s32 y, u32 colour);

internal void draw_line_dda(RenderBackBuffer *buffer, s32 x0, s32 y0, s32 x1, s32 y1, u32 colour);

internal void draw_triangle(RenderBackBuffer *buffer, s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2, u32 colour);

internal void draw_triangle_pixel(RenderBackBuffer *buffer, s32 x, s32 y, Vec4F32 point_a, Vec4F32 point_b, Vec4F32 point_c, u32 colour);

internal void draw_filled_triangle(RenderBackBuffer *buffer, s32 x0, s32 y0, f32 z0, f32 w0, s32 x1, s32 y1, f32 z1, f32 w1, s32 x2, s32 y2, f32 z2, f32 w2, u32 colour);

internal void draw_texel(RenderBackBuffer *buffer, s32 x, s32 y, Vec4F32 point_a, Vec4F32 point_b, Vec4F32 point_c, Texture2F32 a_uv, Texture2F32 b_uv, Texture2F32 c_uv, u32 *texture);

internal void draw_textured_triangle(RenderBackBuffer *buffer, s32 x0, s32 y0, f32 z0, f32 w0, f32 u0, f32 v0, s32 x1, s32 y1, f32 z1, f32 w1, f32 u1, f32 v1, s32 x2, s32 y2, f32 z2, f32 w2, f32 u2, f32 v2, u32 *texture);

internal void draw_rect(RenderBackBuffer *buffer, s32 x, s32 y, s32 width, s32 height, u32 colour);

#endif // DRAW_H