// Copyright Frost Gorilla, Inc. All Rights Reserved.

#ifndef RENDER_RASTER_H
#define RENDER_RASTER_H

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
	f32 *z_buffer;
};

internal void r_resize_buffer(Arena *arena, RenderBackBuffer *buffer, s32 width, s32 height);
internal void r_copy_buffer_to_window(HDC device_context, RenderBackBuffer *buffer);
internal void r_clear_colour_buffer(RenderBackBuffer *buffer, u32 colour);
internal void r_clear_z_buffer(RenderBackBuffer *buffer);

#endif // RENDER_RASTER_H
