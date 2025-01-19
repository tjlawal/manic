// Copyright Frost Gorilla, Inc. All Rights Reserved.

internal void draw_dots(RenderBackBuffer *buffer, u32 colour) {}

internal void draw_grid(RenderBackBuffer *buffer, u32 colour) {}

internal void draw_rect(RenderBackBuffer *buffer, s32 x, s32 y, s32 width, s32 height, u32 colour) {
  for (s32 row = 0; row < width; ++row) {
    for (s32 col = 0; col < height; ++col) {
      s32 current_x = x + row;
      s32 current_y = y + col;
      draw_pixel(buffer, current_x, current_y, colour);
    }
  }
}

internal void draw_pixel(RenderBackBuffer *buffer, s32 x, s32 y, u32 colour) {
  if ((x >= 0) && (x < buffer->width) && (y >= 0) && (y < buffer->height)) {
    buffer->colour_buffer[(buffer->width * y) + x] = colour;
  }
}

internal void draw_line_dda(RenderBackBuffer *buffer, s32 x0, s32 y0, s32 x1, s32 y1, u32 colour) {
  s32 delta_x = (x1 - x0);
  s32 delta_y = (y1 - y0);

  // Check for the longest side length
  s32 longest = (abs(delta_x) >= abs(delta_y)) ? abs(delta_x) : abs(delta_y);

  f32 x_inc = delta_x / (f32)longest;
  f32 y_inc = delta_y / (f32)longest;

  f32 current_x = x0;
  f32 current_y = y0;

  for (s32 i = 0; i < longest; ++i) {
    draw_pixel(buffer, round(current_x), round(current_y), colour);
    current_x += x_inc;
    current_y += y_inc;
  }
}

internal void draw_triangle(RenderBackBuffer *buffer, s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2, u32 colour) {
  draw_line_dda(buffer, x0, y0, x1, y1, colour);
  draw_line_dda(buffer, x1, y1, x2, y2, colour);
  draw_line_dda(buffer, x2, y2, x0, y0, colour);
}

// Draw a filled triangle with the flat-top/flat-bottom method
// We split the original triangle in two, half flat-bottom and half flat-top
//
//          (x0,y0)
//            / \
//           /   \
//          /     \
//         /       \
//        /         \
//   (x1,y1)------(Mx,My)
//       \_           \
//          \_         \
//             \_       \
//                \_     \
//                   \    \
//                     \_  \
//                        \_\
//                           \
//                         (x2,y2)
internal void draw_filled_triangle(RenderBackBuffer *buffer, s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2,
                                   u32 colour) {
  // sort vertices by y-coords in ascending arder (y0 < y1 < y2)
  if (y0 > y1) {
    SWAPP(&y0, &y1, s32);
    SWAPP(&x0, &x1, s32);
  }

  if (y1 > y2) {
    SWAPP(&y1, &y2, s32);
    SWAPP(&x1, &x2, s32);
  }

  if (y0 > y1) {
    SWAPP(&y0, &y1, s32);
    SWAPP(&x0, &x1, s32);
  }

  if (y1 == y2) {
    draw_fill_flat_bottom_triangle(buffer, x0, y0, x1, y1, x2, y2, colour);
  } else if (y0 == y1) {
    draw_fill_flat_top_triangle(buffer, x0, y0, x1, y1, x2, y2, colour);
  } else {
    // Calculate the new vertex (Mx, My) using triangle similarity.
    s32 My = y1;
    s32 Mx = (((x2 - x0) * (y1 - y0)) / (y2 - y0)) + x0;

    draw_fill_flat_bottom_triangle(buffer, x0, y0, x1, y1, Mx, My, colour);
    draw_fill_flat_top_triangle(buffer, x1, y1, Mx, My, x2, y2, colour);
  }
}

// Draw a filled a triangle with a flat bottom
//
//        (x0,y0)
//          / \
//         /   \
//        /     \
//       /       \
//      /         \
//  (x1,y1)------(x2,y2)

internal void draw_fill_flat_bottom_triangle(RenderBackBuffer *buffer, s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2,
                                             u32 colour) {
  // Find the two slopes (both triangle legs)
  // NOTE(tijani): The formula for calculating the slopes of the triangle legs
  // are the inverted version of the formula for a straight line so:
  // m' = delta x / delta y

  f32 left_leg = (f32)(x1 - x0) / (y1 - y0);
  f32 right_leg = (f32)(x2 - x0) / (y2 - y0);

  // Start x_start and x_end from the top vertex (x0, y0)
  f32 x_start = x0;
  f32 x_end = x0;

  // Loop the scanlines from the top to bottom
  for (s32 y = y0; y <= y2; ++y) {
    draw_line_dda(buffer, x_start, y, x_end, y, colour);
    x_start += left_leg;
    x_end += right_leg;
  }
}

// Draw a filled a triangle with a flat top
//
//  (x0,y0)------(x1,y1)
//      \         /
//       \       /
//        \     /
//         \   /
//          \ /
//        (x2,y2)

internal void draw_fill_flat_top_triangle(RenderBackBuffer *buffer, s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2,
                                          u32 colour) {
  // Find the two slopes (both triangle legs)
  // NOTE(tijani): The formula for calculating the slopes of the triangle legs
  // are the inverted version of the formula for a straight line so:
  // m' = delta x / delta y

  f32 left_leg = (f32)(x2 - x0) / (y2 - y0);
  f32 right_leg = (f32)(x2 - x1) / (y2 - y1);

  // Start x_start and x_end from the bottom vertex (x2, y2)
  f32 x_start = x2;
  f32 x_end = x2;

  // Loop through all the scanlines from the bottom to top
  for (s32 y = y2; y >= y0; --y) {
    draw_line_dda(buffer, x_start, y, x_end, y, colour);
    x_start -= left_leg;
    x_end -= right_leg;
  }
}

// Draw a textured pixel at position x and y using interpolation
internal void draw_texel(RenderBackBuffer *buffer, s32 x, s32 y, Vec2F32 a, Vec2F32 b, Vec2F32 c, f32 u0, f32 v0,
                         f32 u1, f32 v1, f32 u2, f32 v2, u32 *texture) {
  Vec2F32 point_p = {x, y};
  Vec3F32 weights = barycentric_weights(a, b, c, point_p);
  f32 alpha = weights.x;
  f32 beta = weights.y;
  f32 gamma = weights.z;

  // Interpolate all 'u' and 'v' values using barycentric weights
  f32 interpolated_u = (u0)*alpha + (u1)*beta + (u2)*gamma;
  f32 interpolated_v = (v0)*alpha + (v1)*beta + (v2)*gamma;

  // Make sure the textures don't get overflowed by texture_x and texture_y
  s32 texture_x = abs((s32)(interpolated_u * texture_width));
  s32 texture_y = abs((s32)(interpolated_v * texture_height));
  texture_x = MIN(texture_x, texture_width - 1);
  texture_y = MIN(texture_y, texture_height - 1);

  draw_pixel(buffer, x, y, texture[texture_width * texture_y + texture_x]);
}

// Draw a textured triangle based on a texture array of colors.
// Split the original triangle in two, half flat-bottom and half flat-top.
//
//        v0
//        /\
//       /  \
//      /    \
//     /      \
//   v1--------\
//     \_       \
//        \_     \
//           \_   \
//              \_ \
//                 \\
//                   \
//                    v2
//

internal void draw_textured_triangle(RenderBackBuffer *buffer, s32 x0, s32 y0, f32 u0, f32 v0, s32 x1, s32 y1, f32 u1,
                                     f32 v1, s32 x2, s32 y2, f32 u2, f32 v2, u32 *texture) {
  // NOTE(tijani): Loop through all the pixels of the triangles to be rendered,
  // sort all the vertices from top-to-bottom to make sure they are always in order,
  // i.e. by ascending order in y-coordinate (y0 < y1 < y2).
  if (y0 > y1) {
    SWAPP(&y0, &y1, s32);
    SWAPP(&x0, &x1, s32);
    SWAPP(&u0, &u1, f32);
    SWAPP(&v0, &v1, f32);
  }

  if (y1 > y2) {
    SWAPP(&y1, &y2, s32);
    SWAPP(&x1, &x2, s32);
    SWAPP(&u1, &u2, f32);
    SWAPP(&v1, &v2, f32);
  }

  if (y0 > y1) {
    SWAPP(&y0, &y1, s32);
    SWAPP(&x0, &x1, s32);
    SWAPP(&u0, &u1, f32);
    SWAPP(&v0, &v1, f32);
  }

  // Create vector points after sorting the vertices
  Vec2F32 point_a = {x0, y0};
  Vec2F32 point_b = {x1, y1};
  Vec2F32 point_c = {x2, y2};

  // NOTE(tijani): Render the upper part of the triangle (the flat-bottom)
  // This is the (delta x / delta y)
  f32 left_leg = 0;
  f32 right_leg = 0;

  if (y1 - y0 != 0)
    left_leg = (f32)(x1 - x0) / (abs((y1 - y0)));

  if (y2 - y0 != 0)
    right_leg = (f32)(x2 - x0) / (abs((y2 - y0)));

  if (y1 - y0 != 0) {
    for (s32 y = y0; y <= y1; y++) {
      s32 x_start = x1 + (y - y1) * left_leg;
      s32 x_end = x0 + (y - y0) * right_leg;

      // if x_end is to the left of x_start, SWAP them so x_start is always to the left of x_end.
      if (x_end < x_start) {
        SWAPP(&x_start, &x_end, s32);
      }

      for (s32 x = x_start; x < x_end; x++) {
        // Draw pixel with colour from texture.
        draw_texel(buffer, x, y, point_a, point_b, point_c, u0, v0, u1, v1, u2, v2, texture);
      }
    }
  }

  // Render the bottom part of the triangle (Flat-Top)
  left_leg = 0;
  right_leg = 0;

  if (y2 - y1 != 0) {
    left_leg = (f32)(x2 - x1) / (abs((y2 - y1)));
  }

  if (y2 - y0 != 0) {
    right_leg = (f32)(x2 - x0) / (abs((y2 - y0)));
  }

  if (y2 - y1 != 0) {
    for (s32 y = y1; y <= y2; y++) {
      s32 x_start = x1 + (y - y1) * left_leg;
      s32 x_end = x0 + (y - y0) * right_leg;

      // if x_end is to the left of x_start, SWAP them so x_start is always to the left of x_end.
      if (x_end < x_start) {
        SWAPP(&x_start, &x_end, s32);
      }

      for (s32 x = x_start; x < x_end; x++) {
        // Draw pixel with colour from texture.
        draw_texel(buffer, x, y, point_a, point_b, point_c, u0, v0, u1, v1, u2, v2, texture);
      }
    }
  }
}
