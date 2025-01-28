// Copyright Frost Gorilla, Inc. All Rights Reserved.

///////////////////////////////////////////////////////
// Build Options
#define BUILD_TITLE                        "Manic"
#define BUILD_VERSION_MAJOR                00
#define BUILD_VERSION_MINOR                00
#define BUILD_VERSION_PATCH                BUILD_SVN_REVISION
#define BUILD_RELEASE_PHASE_STRING_LITERAL "Alpha"
#define OS_FEATURE_GRAPHICAL               1
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION

#define _CRT_SECURE_NO_WARNINGS

// clang-format off
////////////////////////////
// Includes

// NOTE(tijani): order is important here

// Thirdparty
#include "third_party/stb_image.h"

// [.h]
#include "base/base_inc.h"
#include "os/os_inc.h"
#include "render/render_inc.h"
#include "mesh/mesh.h"
#include "manic_core.h"
#include "draw/draw.h"

// [.c]
#include "base/base_inc.c"
#include "os/os_inc.c"
#include "render/render_inc.c"
#include "mesh/mesh.c"
#include "draw/draw.c"
// clang-format on

typedef struct SpotLight SpotLight;
struct SpotLight {
  Vec3F32 direction;
  Vec3F32 position;
};

typedef struct GlobalLight GlobalLight;
struct GlobalLight {
  Vec3F32 direction;
};

//////////////////////////////
// Globals
global HDC g_device_context; // TODO(tijani): is there a better way to do this?
global OS_Handle window_handle = {0};
global Triangle2F32 *g_triangles_to_render = NULL;
global RenderState g_render_state = {.cull_mode = CULL_BACK, .render_mode = RENDER_DEFAULT, .debug_overlay = 0};
global RenderBackBuffer g_buffer = {0};
global GlobalLight g_light = {0, 0, 1};
global Vec3F32 g_camera_position = {.x = 0, .y = 0, .z = 0};

// NOTE(tijani): Perspective Projection Matrix Initialization
global f32 g_fov = PI / 3.0; // NOTE(tijani): see unit circle
global f32 g_znear = 1.0;
global f32 g_zfar = 100.0;
global f32 g_aspect_ratio = 0.0;
global Vec2S32 g_dimensions = {0};
global Mat4F32 g_projection_matrix = {0};

internal u32 light_intensity(u32 colour, f32 percentage) {
  CLAMP(percentage, 0, 1);
  u32 alpha = (colour & 0xFF000000);
  u32 red = (colour & 0x00FF0000) * percentage;
  u32 green = (colour & 0x0000FF00) * percentage;
  u32 blue = (colour & 0x000000FF) * percentage;

  u32 new_colour = alpha | (red & 0x00FF0000) | (green & 0x0000FF00) | (blue & 0x000000FF);
  return new_colour;
}

internal b32 frame() {
  b32 quit = 0;
  // u64 frame_start = os_now_microseconds();
  Temp scratch = scratch_begin(0, 0);

  // tijani: do event stuff
  OS_EventList events = os_get_events(scratch.arena, 0);
  for (OS_Event *event = events.first; event != 0; event = event->next) {
    switch (event->event_kind) {
      case (OS_EventKind_WindowClose): {
        quit = 1;
        break;
      }

        // process keypress
      case (OS_EventKind_Press): {
        switch (event->key) {
          case (OS_Key_Q): {
            quit = 1;
            break;
          }

          // render modes
          case (OS_Key_W): {
            g_render_state.render_mode ^= RENDER_WIREFRAME;
            break;
          }

          case (OS_Key_V): {
            g_render_state.render_mode ^= RENDER_VERTEX;
            break;
          }

          case (OS_Key_F): {
            g_render_state.render_mode ^= RENDER_FILL;
            break;
          }

          case (OS_Key_T): {
            g_render_state.render_mode ^= RENDER_TEXTURE;
            break;
          }

          case (OS_Key_N): {
            g_render_state.render_mode ^= RENDER_TEXTURE_WIREFRAME;
            break;
          }

          case (OS_Key_C): {
            g_render_state.cull_mode ^= CULL_BACK;
            break;
          }

          default:
            break;
        }
      }

      default:
        break;
    }
  }

  g_dimensions = os_window_dimension(window_handle);
  r_resize_buffer(scratch.arena, &g_buffer, g_dimensions.x, g_dimensions.y);

  // prepare triangle to render
  {
    // NOTE(tijani): cool mesh animation, don't delete.
    //   g_triangles_to_render = NULL;
    //   g_mesh.rotate.x += 0.01;
    //   g_mesh.rotate.y += 0.01;
    //   g_mesh.rotate.z += 0.01;

    //   g_mesh.scale.x += 0.002;
    //   g_mesh.scale.y += 0.002;
    //   g_mesh.scale.z += 0.02;

    //   g_mesh.translate.x += 0.001;
    //   g_mesh.translate.y += 0.01;
    //   g_mesh.translate.z = 5.0;

    g_triangles_to_render = NULL;
    //g_mesh.rotate.x += 0.01;
     g_mesh.rotate.y += 0.01;
    // g_mesh.rotate.z += 0.01;

    // g_mesh.scale.x += 0.00;
    // g_mesh.scale.y += 0.002;
    // g_mesh.scale.z += 0.02;

    // g_mesh.translate.x += 0.00;
    // g_mesh.translate.y += 0.01;
    g_mesh.translate.z = 5.0;

    // Create scale matrix to multiply mesh vertices
    Mat4F32 scale_matrix = mat4f32_scale(g_mesh.scale.x, g_mesh.scale.y, g_mesh.scale.z);

    // Create translation matrix
    Mat4F32 translation_matrix = mat4f32_translate(g_mesh.translate.x, g_mesh.translate.y, g_mesh.translate.z);

    // Rotation Matrix
    Mat4F32 rotate_matrix_x = mat4f32_rotate_x(g_mesh.rotate.x);
    Mat4F32 rotate_matrix_y = mat4f32_rotate_y(g_mesh.rotate.y);
    Mat4F32 rotate_matrix_z = mat4f32_rotate_z(g_mesh.rotate.z);

    // Loop through all the triangle faces of the mesh
    s32 faces_count = array_length(g_mesh.faces); // @revise the array_length, could do better using arenas?
    for (s32 i = 0; i < faces_count; ++i) {
      Face3S32 current_mesh_face = g_mesh.faces[i];

      Vec3F32 face_vertices[3];
      face_vertices[0] = g_mesh.vertices[current_mesh_face.a - 1];
      face_vertices[1] = g_mesh.vertices[current_mesh_face.b - 1];
      face_vertices[2] = g_mesh.vertices[current_mesh_face.c - 1];

      Vec4F32 transformed_vertices[3];

      // Loop through all the vertices of the current face and apply
      // transformation
      for (s32 j = 0; j < 3; ++j) {
        Vec4F32 transformed_vertex = vec4f32_from_vec3f32(face_vertices[j]);

        // World Matrix
        Mat4F32 world_matrix = mat4f32_identity();

        // Multiply all matrices and load the world matrix

        // NOTE(tijani): Order matters here. First scale, then rotate, then
        // translate. not respecting the orders of matrix transformations would
        // result in things ending up in weird places.

        world_matrix = mat4f32_mul_mat4f32(scale_matrix, world_matrix);

        world_matrix = mat4f32_mul_mat4f32(rotate_matrix_x, world_matrix);
        world_matrix = mat4f32_mul_mat4f32(rotate_matrix_y, world_matrix);
        world_matrix = mat4f32_mul_mat4f32(rotate_matrix_z, world_matrix);

        world_matrix = mat4f32_mul_mat4f32(translation_matrix, world_matrix);

        // Multiply world matrix by original vector
        transformed_vertex = mat4f32_mul_vec4(world_matrix, transformed_vertex);

        // Save transformed vertex in the array of transformed vectices
        transformed_vertices[j] = transformed_vertex;
      }

      // @NOTE: triangles are clockwise.
      Vec3F32 a = vec3f32_from_vec4f32(transformed_vertices[0]); /*   A   */
      Vec3F32 b = vec3f32_from_vec4f32(transformed_vertices[1]); /*  / \  */
      Vec3F32 c = vec3f32_from_vec4f32(transformed_vertices[2]); /* C---B */

      // find vectors (b - a) and (c -a)
      Vec3F32 ab = vec3_sub(b, a);
      Vec3F32 ac = vec3_sub(c, a);
      vec3_normalize(&ab);
      vec3_normalize(&ac);

      // Compute the face normal using cross product to find perpendicular.
      Vec3F32 normal = vec3_cross(ab, ac);
      vec3_normalize(&normal);

      // Find vector between point in the triangle and the camera origin
      Vec3F32 camera_ray = vec3_sub(g_camera_position, a);

      // if face normal (dot_product) is aligned with camera ray, draw if not
      // cull (dont draw).
      f32 dot_normal_camera = vec3_dot(normal, camera_ray);

      // check backface culling
      {
        if (g_render_state.cull_mode & CULL_BACK) {
          // Dont render if looking away  from the camera, remember we are a
          // right handed coordinate system.
          if (dot_normal_camera < 0) {
            continue;
          }
        }
      }

      Vec4F32 projected_points[3];
      // Loop through all the vectices and perform projection
      for (s32 k = 0; k < 3; ++k) {
        // Project the current vertex
        projected_points[k] = mat4f32_mul_projection(g_projection_matrix, transformed_vertices[k]);

        // Scale into the viewport
        projected_points[k].x *= (g_buffer.width / 2.0);
        projected_points[k].y *= (g_buffer.height / 2.0);

        // printf("ProjectedPoints: x - %f, y - %f\n", projected_points[k].x,
        // projected_points[k].y);

        // Translate projected points to the middle of the screen.
        projected_points[k].x += (g_buffer.width / 2.0);
        projected_points[k].y += (g_buffer.height / 2.0);
      }

      // Calculate light intensity based on the alignment of the face normal and
      // the light ray.
      f32 light_intensity_factor = -vec3_dot(normal, g_light.direction);
      u32 triangle_colour = light_intensity(current_mesh_face.colour, light_intensity_factor);

      Triangle2F32 projected_triangle = {
          .points = {
						{projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w},
						{projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w},
						{projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w},
					},
          .texture_coords = {
						{current_mesh_face.a_uv.u, current_mesh_face.a_uv.v},
						{current_mesh_face.b_uv.u, current_mesh_face.b_uv.v},
						{current_mesh_face.c_uv.u, current_mesh_face.c_uv.v},
					},
          .colour = triangle_colour
			};

			// TODO: use arena here!!
      // save projected triangle in array of triangles to be rendered
      array_push(g_triangles_to_render, projected_triangle);
    }
  }

  // NOTE(tijani): Now draw
  { // ARGB
    r_clear_colour_buffer(&g_buffer, 0xFF2C2C2C);
    r_clear_z_buffer(&g_buffer);

    // Loop through all projected points and render
    int triangle_count = array_length(g_triangles_to_render);

    for (u32 i = 0; i < triangle_count; ++i) {
      Triangle2F32 triangle = g_triangles_to_render[i];

      // Draw vertex points
      if (g_render_state.render_mode & RENDER_VERTEX) {
        draw_rect(&g_buffer, triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFE7104); // Vertex A
        draw_rect(&g_buffer, triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFE7104); // Vertex B
        draw_rect(&g_buffer, triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFE7104); // Vertex C
      }

      // Draw filled triangle
      if (g_render_state.render_mode == RENDER_DEFAULT || g_render_state.render_mode & RENDER_FILL) {
        draw_filled_triangle(
            &g_buffer, triangle.points[0].x, triangle.points[0].y, triangle.points[0].z,
            triangle.points[0].w,                                                                   // Vertex A
            triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, // Vertex B
            triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, // Vertex C
            triangle.colour);
      }

      // Draw just wireframe or textured wireframe
      if (g_render_state.render_mode & RENDER_WIREFRAME || g_render_state.render_mode & RENDER_TEXTURE_WIREFRAME) {
        draw_triangle(&g_buffer, triangle.points[0].x, triangle.points[0].y, // Vertex A
                      triangle.points[1].x, triangle.points[1].y,            // Vertex B
                      triangle.points[2].x, triangle.points[2].y,            // Vertex C
                      0xFFFFFFF);
      }

      // Draw textured triangle
      if (g_render_state.render_mode & RENDER_TEXTURE) {
        draw_textured_triangle(&g_buffer, triangle.points[0].x, triangle.points[0].y, triangle.points[0].z,
                               triangle.points[0].w, triangle.texture_coords[0].u,
                               triangle.texture_coords[0].v, // Vertex A
                               triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,
                               triangle.texture_coords[1].u, triangle.texture_coords[1].v, // Vertex B
                               triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,
                               triangle.texture_coords[2].u, triangle.texture_coords[2].v, // Vertex C
                               g_texture_mesh);
      }
    }
		// TODO: Revise this, to use the scratch memory already allocated per-frame.
		// Just need a way to load-up dynamic arrays into it.
		array_free(g_triangles_to_render);
  }

  // NOTE(tijani): Send back-buffer to window.
  r_copy_buffer_to_window(g_device_context, &g_buffer);

  scratch_end(scratch);
  return quit;
}

///////////////////////////////////////////////////
// Entrypoint

void entry_point() {
  window_handle = os_window_open(V2S32(900, 800), 0, str8_lit(BUILD_TITLE_STRING_LITERAL));
  os_window_first_paint(window_handle);
  g_device_context = os_get_device_context(window_handle);

  // Initialize the perspective matrix
  g_dimensions = os_window_dimension(window_handle);
  g_aspect_ratio = (f32)g_dimensions.x / (f32)g_dimensions.y;
  g_projection_matrix = mat4f32_perspective_project(g_fov, g_aspect_ratio, g_znear, g_zfar);

  load_obj_file_data_from_file("data/meshes/drone.obj");
  load_texture_data("data/textures/drone.png");

  // equip renderer
  for (; !update();) {
    Sleep(1); // DON'T MELT DOWN THE CPU, DUFUS.
  }
}