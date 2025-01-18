// Copyright Frost Gorilla, Inc. All Rights Reserved.

#ifndef DRAW_H
#define DRAW_H

internal void draw_pixel(RenderBackBuffer *buffer, s32 x, s32 y, u32 colour);
internal void draw_line_dda(RenderBackBuffer *buffer, s32 x0, s32 y0, s32 x1, s32 y1, u32 colour);
internal void draw_triangle(RenderBackBuffer *buffer, s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2, u32 colour);
internal void draw_filled_triangle(RenderBackBuffer *buffer, s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2,
                                   u32 colour);
internal void draw_fill_flat_bottom_triangle(RenderBackBuffer *buffer, s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2,
                                             u32 colour);
internal void draw_fill_flat_top_triangle(RenderBackBuffer *buffer, s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2,
                                          u32 colour);
internal void draw_texel(RenderBackBuffer *buffer, s32 x, s32 y, Vec2F32 a, Vec2F32 b, Vec2F32 c, f32 u0, f32 v0,
                         f32 u1, f32 v1, f32 u2, f32 v2, u32 *texture);
internal void draw_textured_triangle(RenderBackBuffer *buffer, s32 x0, s32 y0, f32 u0, f32 v0, s32 x1, s32 y1, f32 u1,
                                     f32 v1, s32 x2, s32 y2, f32 u2, f32 v2, u32 *texture);
internal void draw_dots(RenderBackBuffer *buffer, u32 colour);
internal void draw_grid(RenderBackBuffer *buffer, u32 colour);
internal void draw_rect(RenderBackBuffer *buffer, s32 x, s32 y, s32 width, s32 height, u32 colour);

#endif // DRAW_H