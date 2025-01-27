// Copyright Frost Gorilla, Inc. All Rights Reserved.

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

internal void draw_triangle_pixel(RenderBackBuffer *buffer, s32 x, s32 y, Vec4F32 point_a, Vec4F32 point_b, Vec4F32 point_c, u32 colour){
	Vec2F32 point_p = {x, y};

  Vec2F32 a = vec2f32_from_vec4f32(point_a);
  Vec2F32 b = vec2f32_from_vec4f32(point_b);
  Vec2F32 c = vec2f32_from_vec4f32(point_c);

  Vec3F32 weights = barycentric_weights(a, b, c, point_p);
  f32 alpha = weights.x;
  f32 beta = weights.y;
  f32 gamma = weights.z;

  // Interpolated the value of 1/W for the current pixel.
  f32 interpolated_reciprocal_w;

	// Interpolate the value of 1/w for teh current pixel
	interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

	// NOTE(tijani): this is nasty
	// Adjust 1/w so that the pixel that are close to the camera have smaller values
	interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

  // Only draw pixel and update z-buffer if the depth is less that the value that already exists in the z-buffer
	if(interpolated_reciprocal_w < buffer->z_buffer[(buffer->width * y) + x] ){
		draw_pixel(buffer, x, y, colour);

		// update the z-buffer with the 1/w of the current pixel
		buffer->z_buffer[(buffer->width * y) + x] = interpolated_reciprocal_w;
	}
}

internal void draw_filled_triangle(RenderBackBuffer *buffer, s32 x0, s32 y0, f32 z0, f32 w0, s32 x1, s32 y1, f32 z1, f32 w1, s32 x2, s32 y2, f32 z2, f32 w2, u32 colour){
	if (y0 > y1) {
    SWAPP(&y0, &y1, s32);
    SWAPP(&x0, &x1, s32);
    SWAPP(&z0, &z1, f32);
    SWAPP(&w0, &w1, f32);
  }

  if (y1 > y2) {
    SWAPP(&y1, &y2, s32);
    SWAPP(&x1, &x2, s32);
    SWAPP(&z1, &z2, f32);
    SWAPP(&w1, &w2, f32);
  }

  if (y0 > y1) {
    SWAPP(&y0, &y1, s32);
    SWAPP(&x0, &x1, s32);
    SWAPP(&z0, &z1, f32);
    SWAPP(&w0, &w1, f32);
  }

	// Create vector points after sorting the vertices
  Vec4F32 point_a = {x0, y0, z0, w0};
  Vec4F32 point_b = {x1, y1, z1, w1};
  Vec4F32 point_c = {x2, y2, z2, w2};

	// Draw filled flat-bottom of triangle
	f32 left_leg = 0;
	f32 right_leg = 0;

	if((y1 - y0) != 0)
		left_leg = (f32)(x1 - x0) / abs(y1 - y0);

	if((y2 - y0) != 0)
		right_leg = (f32)(x2 - x0) / (abs(y2 - y0));

	if((y1 - y0) != 0) {
		for(s32 y = y0; y <= y1; ++y){
			s32 x_start = x1 + (y - y1) * left_leg;
			s32 x_end = x0 + (y - y0) * right_leg;

			if(x_end < x_start){
				SWAPP(&x_start, &x_end, s32);
			}

			for(s32 x = x_start; x < x_end; ++x){
				draw_triangle_pixel(buffer, x, y, point_a, point_b, point_c, colour);
			}
		}
	}

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
        draw_triangle_pixel(buffer, x, y, point_a, point_b, point_c, colour);
      }
    }
  }
}

// Draw a textured pixel at position x and y using interpolation
// TODO(tijani): THIS IS SLOW, SPEED UP!!!
internal void draw_texel(RenderBackBuffer *buffer, s32 x, s32 y, Vec4F32 point_a, Vec4F32 point_b, Vec4F32 point_c,  Texture2F32 a_uv, Texture2F32 b_uv, Texture2F32 c_uv,  u32 *texture) {
  Assert(texture != NULL);
  Vec2F32 point_p = {x, y};

  Vec2F32 a = vec2f32_from_vec4f32(point_a);
  Vec2F32 b = vec2f32_from_vec4f32(point_b);
  Vec2F32 c = vec2f32_from_vec4f32(point_c);

  Vec3F32 weights = barycentric_weights(a, b, c, point_p);
  f32 alpha = weights.x;
  f32 beta = weights.y;
  f32 gamma = weights.z;

  // Interpolated values of U, V, and 1/W for the current pixel.
  f32 interpolated_u;
  f32 interpolated_v;
  f32 interpolated_reciprocal_w;

  // Interpolate all 'u/w' and 'v/w' values using barycentric weights and a factuor of 1/w
  interpolated_u = ((a_uv.u / point_a.w)) * alpha + ((b_uv.u / point_b.w)) * beta + ((c_uv.u / point_c.w)) * gamma;
  interpolated_v = ((a_uv.v / point_a.w)) * alpha + ((b_uv.v / point_b.w)) * beta + ((c_uv.v / point_c.w)) * gamma;

  // Interpolate the value of 1/w for the current pixel
  interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

  // Divide interpolated values attributes by 1/w (this undoes the perspective transform)
  interpolated_u /= interpolated_reciprocal_w;
  interpolated_v /= interpolated_reciprocal_w;

  // Make sure the textures don't get overflowed by texture_x and texture_y
  s32 texture_x = abs((s32)(interpolated_u * g_texture_width));
  s32 texture_y = abs((s32)(interpolated_v * g_texture_height));

  texture_x = MIN(texture_x, g_texture_width - 1);
  texture_y = MIN(texture_y, g_texture_height - 1);

  // NOTE(tijani): stb_image returns loaded png texture data in RGBA format,
  // StretchDIBits in windows uses only BGRA format, hence we need to pack it.

  // TODO(tijani): Make this os independent!
  u32 pixel_colour = texture[(g_texture_width * texture_y) + texture_x];
  u32 packed_pixel_colour = ((pixel_colour >> 16) & 0xFF) |      // B
                            ((pixel_colour >> 8) & 0xFF) << 8 |  // G
                            ((pixel_colour >> 0) & 0xFF) << 16 | // R
                            ((pixel_colour >> 24) & 0xFF) << 24; // A

	// NOTE(tijani): this is nasty
	// Adjust 1/w so that the pixel that are close to the camera have smaller values
	interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

  // Only draw pixel and update z-buffer if the depth is less that the value that already exists in the z-buffer
  if (interpolated_reciprocal_w < buffer->z_buffer[(buffer->width * y) + x]) {
    draw_pixel(buffer, x, y, packed_pixel_colour);

    // Update the z-buffer value withthe 1/w of this current pixel
    buffer->z_buffer[(buffer->width * y) + x] = interpolated_reciprocal_w;
  }
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

internal void draw_textured_triangle(RenderBackBuffer *buffer, 
																		 s32 x0, s32 y0, f32 z0, f32 w0, f32 u0, f32 v0, 
																		 s32 x1, s32 y1, f32 z1, f32 w1, f32 u1, f32 v1, 
																		 s32 x2, s32 y2, f32 z2, f32 w2, f32 u2, f32 v2, 
																		 u32 *texture) {

  // NOTE(tijani): Loop through all the pixels of the triangles to be rendered,
  // sort all the vertices from top-to-bottom to make sure they are always in order,
  // i.e. by ascending order in y-coordinate (y0 < y1 < y2).
  if (y0 > y1) {
    SWAPP(&y0, &y1, s32);
    SWAPP(&x0, &x1, s32);
    SWAPP(&z0, &z1, f32);
    SWAPP(&w0, &w1, f32);
    SWAPP(&u0, &u1, f32);
    SWAPP(&v0, &v1, f32);
  }

  if (y1 > y2) {
    SWAPP(&y1, &y2, s32);
    SWAPP(&x1, &x2, s32);
    SWAPP(&z1, &z2, f32);
    SWAPP(&w1, &w2, f32);
    SWAPP(&u1, &u2, f32);
    SWAPP(&v1, &v2, f32);
  }

  if (y0 > y1) {
    SWAPP(&y0, &y1, s32);
    SWAPP(&x0, &x1, s32);
    SWAPP(&z0, &z1, f32);
    SWAPP(&w0, &w1, f32);
    SWAPP(&u0, &u1, f32);
    SWAPP(&v0, &v1, f32);
  }

  // NOTE(tijani): Flip V component to account for inverted UV-coordinates.
  // sometimes, some obj files grow their v downward, we grow ours upward.
  v0 = (1.0 - v0);
  v1 = (1.0 - v1);
  v2 = (1.0 - v2);

  // Create vector points after sorting the vertices
  Vec4F32 point_a = {x0, y0, z0, w0};
  Vec4F32 point_b = {x1, y1, z1, w1};
  Vec4F32 point_c = {x2, y2, z2, w2};

  Texture2F32 a_uv = {u0, v0};
  Texture2F32 b_uv = {u1, v1};
  Texture2F32 c_uv = {u2, v2};

  // NOTE(tijani): Render the upper part of the triangle (the flat-bottom)
  // This is the (delta x / delta y)
  f32 left_leg = 0;
  f32 right_leg = 0;

  if (y1 - y0 != 0)
    left_leg = (f32)(x1 - x0) / (abs(y1 - y0));

  if (y2 - y0 != 0)
    right_leg = (f32)(x2 - x0) / (abs(y2 - y0));

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
        draw_texel(buffer, x, y, point_a, point_b, point_c, a_uv, b_uv, c_uv, texture);
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
        draw_texel(buffer, x, y, point_a, point_b, point_c, a_uv, b_uv, c_uv, texture);
      }
    }
  }
}
