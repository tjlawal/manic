// Copyright Frost Gorilla, Inc. All Rights Reserved.

#ifndef RENDER_SOFTWARE_H
#define RENDER_SOFTWARE_H

// TODO(tijani): this is too win32 specific. make it platform agnostic.
typedef struct RenderBackBuffer RenderBackBuffer;
struct RenderBackBuffer {
  BITMAPINFO info;
  void *memory;
  s32 width;
  s32 height;
  s32 pitch; // TODO(tijani): find out if I actual need this??
  s32 bytes_per_pixel;
  u32 *colour_buffer;
};

internal void r_resize_buffer(Arena *arena, RenderBackBuffer *buffer, s32 width, s32 height);
internal void r_copy_buffer_to_window(HDC device_context, RenderBackBuffer *buffer);

internal void r_draw_pixel(RenderBackBuffer *buffer, s32 x, s32 y, u32 colour);
internal void r_clear_colour_buffer(RenderBackBuffer *buffer, u32 colour);
internal void r_draw_line_dda(RenderBackBuffer *buffer, s32 x0, s32 y0, s32 x1, s32 y1, u32 colour);
internal void r_draw_triangle(RenderBackBuffer *buffer, s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2, u32 colour);
internal void r_draw_filled_triangle(RenderBackBuffer *buffer, s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2,
                                     u32 colour);
internal void r_draw_fill_flat_bottom_triangle(RenderBackBuffer *buffer, s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2,
                                               u32 colour);
internal void r_draw_fill_flat_top_triangle(RenderBackBuffer *buffer, s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2,
                                            u32 colour);

void r_draw_dots(RenderBackBuffer *buffer, u32 colour);
void r_draw_grid(RenderBackBuffer *buffer, u32 colour);
void r_draw_rect(RenderBackBuffer *buffer, s32 x, s32 y, s32 width, s32 height, u32 colour);

// Utilities
internal void swap_ints(s32 *a, s32 *b) {
  s32 temp = *a;
  *a = *b;
  *b = temp;
}

#endif // RENDER_SOFTWARE_H