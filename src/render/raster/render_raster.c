// Copyright Frost Gorilla, Inc. All Rights Reserved.

// TODO(tijani): this function is too os specific.
internal void r_resize_buffer(Arena *arena, RenderBackBuffer *buffer, s32 width, s32 height) {
  buffer->width = width;
  buffer->height = height;
  buffer->bytes_per_pixel = 4;
  buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
  buffer->info.bmiHeader.biWidth = buffer->width;
  // NOTE(tijani): This would be negative when going top-down
  buffer->info.bmiHeader.biHeight = buffer->height;
  buffer->info.bmiHeader.biPlanes = 1;
  buffer->info.bmiHeader.biBitCount = 32;
  buffer->info.bmiHeader.biCompression = BI_RGB;

  s32 bitmap_memory_size = (buffer->width * buffer->height) * buffer->bytes_per_pixel;
  // NOTE(tijani): using u
  buffer->memory = push_array(arena, u8, bitmap_memory_size);
  buffer->colour_buffer = (u32 *)buffer->memory;
}

// TODO(tijani): This is too OS specific.
internal void r_copy_buffer_to_window(HDC device_context, RenderBackBuffer *buffer) {
  StretchDIBits(device_context, 0, 0, buffer->width, buffer->height, 0, 0, buffer->width, buffer->height,
                buffer->memory, &buffer->info, DIB_RGB_COLORS, SRCCOPY);
}

internal void r_draw_pixel(RenderBackBuffer *buffer, s32 x, s32 y, u32 colour) {
  if ((x >= 0) && (x < buffer->width) && (y >= 0) && (y < buffer->height)) {
    buffer->colour_buffer[(buffer->width * y) + x] = colour;
  }
}

void r_clear_colour_buffer(RenderBackBuffer *buffer, u32 colour) {
  for (int y = 0; y < buffer->height; y++) {
    for (int x = 0; x < buffer->width; x++) {
      buffer->colour_buffer[(buffer->width * y) + x] = colour;
    }
  }
}

void r_draw_line_dda(RenderBackBuffer *buffer, s32 x0, s32 y0, s32 x1, s32 y1, u32 colour) {
  s32 delta_x = (x1 - x0);
  s32 delta_y = (y1 - y0);

  // NOTE(tijani): Check the longest
  s32 longest_side_length = (abs(delta_x) >= abs(delta_y)) ? abs(delta_x) : abs(delta_y);

  f32 x_increment = delta_x / (f32)longest_side_length;
  f32 y_increment = delta_y / (f32)longest_side_length;

  f32 current_x = x0;
  f32 current_y = y0;
  for (int i = 0; i < longest_side_length; i++) {
    r_draw_pixel(buffer, round(current_x), round(current_y), colour);
    current_x += x_increment;
    current_y += y_increment;
  }
}

void r_draw_triangle(RenderBackBuffer *buffer, s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2, u32 colour) {
  r_draw_line_dda(buffer, x0, y0, x1, y1, colour);
  r_draw_line_dda(buffer, x1, y1, x2, y2, colour);
  r_draw_line_dda(buffer, x2, y2, x0, y0, colour);
}

internal void r_draw_filled_triangle(RenderBackBuffer *buffer, s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2,
                                     u32 colour) {
  // Sort vertices by y-coordinate in ascending order (y0 < y1 < y2)
  if (y0 > y1) {
    swap(&y0, &y1, s32);
    swap(&x0, &x1, s32);
  }

  if (y1 > y2) {
    swap(&y1, &y2, s32);
    swap(&x1, &x2, s32);
  }

  if (y0 > y1) {
    swap(&y0, &y1, s32);
    swap(&x0, &x1, s32);
  }

  // NOTE(tijani): in a case where the middle vertex is equal to the bottom half of the triangle,
  // just draw the flat-bottom triangle
  if (y1 == y2) {
    r_draw_fill_flat_bottom_triangle(buffer, x0, y0, x1, y1, x2, y2, colour);
  }
  // NOTE(tijani): In a different case where the top triangle is rotated such that y0 == y1,
  // then it is a flat-top triangle.
  else if (y0 == y1) {
    r_draw_fill_flat_top_triangle(buffer, x0, y0, x1, y1, x2, y2, colour);
  }
  // NOTE(tijani): in the case were we have a normal triangle, then we just drow teh flat-top and flat-bottom triangle
  else {
    // Calculate the new vertex (Mx, My) using triangle similarity.
    s32 My = y1;
    s32 Mx = (((f32)(x2 - x0) * (y1 - y0)) / (f32)(y2 - y0)) + x0;

    r_draw_fill_flat_bottom_triangle(buffer, x0, y0, x1, y1, Mx, My, colour);
    r_draw_fill_flat_top_triangle(buffer, x1, y1, Mx, My, x2, y2, colour);
  }
}

// NOTE(tijani): Draw a textured triangle.
// The flat-top/flat-bottom algorithm is still used here to split the triangle into two halfs
internal void r_draw_textured_triangle(RenderBackBuffer *buffer, s32 x0, s32 y0, f32 u0, f32 v0, s32 x1, s32 y1, f32 u1,
                                       f32 v1, s32 x2, s32 y2, f32 u2, f32 v2, u32 *texture) {
  // NOTE(tijani): Loop through all the pixels of the triangles to be rendered,
  // sort all the vertices from top-to-bottom to make sure they are always in order,
  // i.e. by ascending order in y-coordinate (y0 < y1 < y2).
  if (y0 > y1) {
    swap(&y0, &y1, s32);
    swap(&x0, &x1, s32);
    swap(&u0, &u1, f32);
    swap(&v0, &v1, f32);
  }

  if (y1 > y2) {
    swap(&y1, &y2, s32);
    swap(&x1, &x2, s32);
    swap(&u1, &u2, f32);
    swap(&v1, &v2, f32);
  }

  if (y0 > y1) {
    swap(&y0, &y1, s32);
    swap(&x0, &x1, s32);
    swap(&u0, &u1, f32);
    swap(&v0, &v1, f32);
  }

  // NOTE(tijani): Render the upper part of the triangle (the flat-bottom)
  // This is the (delta x / delta y)
  f32 inverse_left_leg = 0;
  f32 inverse_right_leg = 0;

  if (y1 - y0 != 0)
    inverse_left_leg = (f32)(x1 - x0) / (abs((y1 - y0)));

  if (y2 - y0 != 0)
    inverse_right_leg = (f32)(x2 - x0) / (abs((y2 - y0)));

  //printf("%f\n", inverse_left_leg);
  //printf("%f\n", inverse_right_leg);

  if (y1 - y0 != 0) {
    for (s32 y = y0; y <= y1; y++) {
      s32 x_start = x1 + (y - y1) * inverse_left_leg;
      s32 x_end = x0 + (y - y0) * inverse_right_leg;

      // if x_end is to the left of x_start, swap them so x_start is always to the left of x_end.
      if (x_end < x_start) {
        swap(&x_start, &x_end, s32);
      }

      for (s32 x = x_start; x < x_end; x++) {
        // Draw pixel with colour from texture.
        r_draw_pixel(buffer, x, y, (x % 2 == 0 && y % 2 == 0) ? 0xFF380772 : 0xFF138112);
      }
    }
  }

  // Render the bottom part of the triangle (Flat-Top)
  inverse_left_leg = 0;
  inverse_right_leg = 0;

  if (y2 - y1 != 0) {
    inverse_left_leg = (f32)(x2 - x1) / (abs((y2 - y1)));
  }

  if (y2 - y0 != 0) {
    inverse_right_leg = (f32)(x2 - x0) / (abs((y2 - y0)));
  }

  if (y2 - y1 != 0) {
    for (s32 y = y1; y <= y2; y++) {
      s32 x_start = x1 + (y - y1) * inverse_left_leg;
      s32 x_end = x0 + (y - y0) * inverse_right_leg;

      // if x_end is to the left of x_start, swap them so x_start is always to the left of x_end.
      if (x_end < x_start) {
        swap(&x_start, &x_end, s32);
      }

      for (s32 x = x_start; x < x_end; x++) {
        // Draw pixel with colour from texture.
        // r_draw_pixel(buffer, x, y, 0xFFFF00FF);
        r_draw_pixel(buffer, x, y, (x % 2 == 0 && y % 2 == 0) ? 0xFF380772 : 0xFF138112);
      }
    }
  }
}

internal void r_draw_fill_flat_bottom_triangle(RenderBackBuffer *buffer, s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2,
                                               u32 colour) {

  // NOTE(tijani): The formula for calculating the slopes of the triangle legs
  // are the inverted version of the formula for a straight line so:
  // m' = delta x / delta y
  f32 left_leg = (f32)(x1 - x0) / (y1 - y0);
  f32 right_leg = (f32)(x2 - x0) / (y2 - y0);

  // Start x_start and x_end from the top vertex (x0, y0)
  f32 x_start = x0;
  f32 x_end = x0;

  // loop from top to bottom
  for (s32 y = y0; y <= y2; ++y) {
    r_draw_line_dda(buffer, x_start, y, x_end, y, colour);
    x_start += left_leg;
    x_end += right_leg;
  }
}

internal void r_draw_fill_flat_top_triangle(RenderBackBuffer *buffer, s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2,
                                            u32 colour) {
  // NOTE(tijani): The formula for calculating the slopes of the triangle legs
  // are the inverted version of the formula for a straight line so:
  // m' = delta x / delta y
  f32 left_leg = (f32)(x2 - x0) / (y2 - y0);
  f32 right_leg = (f32)(x2 - x1) / (y2 - y1);

  // Start x_start and x_end from the bottom vertex (x2, y2)
  f32 x_start = x2;
  f32 x_end = x2;

  // loop from bottom to top
  for (s32 y = y2; y >= y0; --y) {
    r_draw_line_dda(buffer, x_start, y, x_end, y, colour);
    x_start -= left_leg;
    x_end -= right_leg;
  }
}

void r_draw_dots(RenderBackBuffer *buffer, u32 colour) {
  for (s32 column = 0; column < buffer->height; column += 10) {
    for (s32 row = 0; row < buffer->width; row += 10) {
      buffer->colour_buffer[(buffer->width * column) + row] = colour;
    }
  }
}

void r_draw_grid(RenderBackBuffer *buffer, u32 colour) {
  for (s32 column = 0; column < buffer->height; column += 10) {
    for (s32 row = 0; row < buffer->width; row += 10) {
      buffer->colour_buffer[(buffer->width * column) + row] = colour;
    }
  }
}

void r_draw_rect(RenderBackBuffer *buffer, s32 x, s32 y, s32 width, s32 height, u32 colour) {
  for (int row = 0; row < width; row++) {
    for (int column = 0; column < height; column++) {
      s32 l_row = row + x;
      s32 l_column = column + y;

      r_draw_pixel(buffer, l_row, l_column, colour);
    }
  }
}
