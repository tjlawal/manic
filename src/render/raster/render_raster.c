// Copyright Frost Gorilla, Inc. All Rights Reserved.

// TODO(tijani): this function is too os specific.
internal void r_resize_buffer(Arena *arena, RenderBackBuffer *buffer, s32 width, s32 height) {
  buffer->width = width;
  buffer->height = height;
  buffer->bytes_per_pixel = 4;
  buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
  buffer->info.bmiHeader.biWidth = buffer->width;
  buffer->info.bmiHeader.biHeight = buffer->height; // This would be negative when going top-down
  buffer->info.bmiHeader.biPlanes = 1;
  buffer->info.bmiHeader.biBitCount = 32;
  buffer->info.bmiHeader.biCompression = BI_RGB;

  s32 bitmap_memory_size = (buffer->width * buffer->height) * buffer->bytes_per_pixel;

  buffer->memory = push_array(arena, u8, bitmap_memory_size);
  buffer->colour_buffer = (u32 *)buffer->memory;

	// NOTE(tijani): is this a good way to do this?
  f32 z_buffer_memory_size = (buffer->width * buffer->height) * buffer->bytes_per_pixel;
  f32 *z_buffer_memory = push_array(arena, f32, z_buffer_memory_size);

  buffer->z_buffer = z_buffer_memory;
}

// TODO(tijani): This is too OS specific.
internal void r_copy_buffer_to_window(HDC device_context, RenderBackBuffer *buffer) {
  StretchDIBits(device_context, 0, 0, buffer->width, buffer->height, 0, 0, buffer->width, buffer->height,
                buffer->memory, &buffer->info, DIB_RGB_COLORS, SRCCOPY);
}

// TODO(tijani): This is stupid and could be so much faster.
internal void r_clear_colour_buffer(RenderBackBuffer *buffer, u32 colour) {
  for (s32 y = 0; y < buffer->height; y++) {
    for (s32 x = 0; x < buffer->width; x++) {
      buffer->colour_buffer[(buffer->width * y) + x] = colour;
    }
  }
}

// TODO(tijani): This is stupid and could be so much faster.
internal void r_clear_z_buffer(RenderBackBuffer *buffer) {
  for (s32 y = 0; y < buffer->height; ++y) {
    for (s32 x = 0; x < buffer->width; ++x) {
      buffer->z_buffer[(buffer->width * y) + x] = 1.0f;
    }
  }
}
