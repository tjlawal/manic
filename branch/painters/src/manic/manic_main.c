// Copyright Frost Gorilla, Inc. All Rights Reserved.

///////////////////////////////////////////////////////
// Build Options
#define BUILD_TITLE          "Manic Renderer"
#define OS_FEATURE_GRAPHICAL 1

// clang-format off

////////////////////////////
// Includes

// NOTE(tijani): order is important here

// [.h]
#include "base/base_inc.h"
#include "os/os_inc.h"
#include "camera/camera_inc.h"
#include "render/render_inc.h"
#include "mesh/mesh_inc.h"

// [.c]
#include "base/base_inc.c"
#include "os/os_inc.c"
#include "camera/camera_inc.c"
#include "render/render_inc.c"
#include "mesh/mesh_inc.c"

// clang-format on

//////////////////////////////
// Globals
// @Revise: is there a better way to do this?
HDC device_context;
global OS_Handle window_handle = {0};
global Triangle2F32 *g_triangles_to_render = NULL;
global RenderState g_render_state = {.cull_mode = CULL_BACK, .render_mode = RENDER_DEFAULT, .debug_overlay = 0};

internal b32 frame() {
  b32 quit = 0;
  Temp scratch = scratch_begin(0, 0);

  // do event stuff
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
          case (OS_Key_A): {
            // TODO(tijani): handle the exception thrown here on the os layer
            *(int *)0 = 0;
            break;
          }
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

  // setup buffer
  Vec2S32 dimensions = os_window_dimension(window_handle);
  RenderBackBuffer buffer = {0};
  r_resize_buffer(scratch.arena, &buffer, dimensions.x, dimensions.y);

  // prepare triangle to render
  {
    g_triangles_to_render = NULL;
    g_mesh.rotation.x += 0.01;
    g_mesh.rotation.y += 0.01;
    g_mesh.rotation.z += 0.0;

    // Loop through all the triangle faces of the mesh
    s32 faces_count = array_length(g_mesh.faces); // @revise the array_length, could do better using arenas?
    for (s32 i = 0; i < faces_count; ++i) {
      Face3S32 current_mesh_face = g_mesh.faces[i];

      Vec3F32 face_vertices[3];
      face_vertices[0] = g_mesh.vertices[current_mesh_face.a - 1];
      face_vertices[1] = g_mesh.vertices[current_mesh_face.b - 1];
      face_vertices[2] = g_mesh.vertices[current_mesh_face.c - 1];

      Vec3F32 transformed_vertices[3];

      // Loop through all the vertices of the current face and apply
      // transformation
      for (s32 j = 0; j < 3; ++j) {
        Vec3F32 transformed_vertex = face_vertices[j];

        transformed_vertex = vec3f32_rotate_x(transformed_vertex, g_mesh.rotation.x);
        transformed_vertex = vec3f32_rotate_y(transformed_vertex, g_mesh.rotation.y);
        transformed_vertex = vec3f32_rotate_z(transformed_vertex, g_mesh.rotation.z);

        // Translate the vertex away from the camera
        transformed_vertex.z += 5;
        transformed_vertices[j] = transformed_vertex;
      }

      // check backface culling
      if (g_render_state.cull_mode & CULL_BACK) {
        // @NOTE: triangles are clockwise.
        // find vectors (b - a) and (c -a)
        Vec3F32 a = transformed_vertices[0]; /*   A   */
        Vec3F32 b = transformed_vertices[1]; /*  / \  */
        Vec3F32 c = transformed_vertices[2]; /* C---B */

        Vec3F32 ab = vec3_sub(b, a);
        Vec3F32 ac = vec3_sub(c, a);

        // Compute the face normal using cross product to find perpendicular.
        Vec3F32 normal = vec3_cross(ab, ac);

        // Find vector between point in the triangle and the camera origin
        Vec3F32 camera_ray = vec3_sub(camera_position, a);

        // if face normal (dot_product) is aligned with camera ray, draw if not
        // cull (dont draw).
        f32 dot_normal_camera = vec3_dot(normal, camera_ray);

        // Dont render if looking away  from the camera, remember we are a right
        // handed coordinate system.
        if (dot_normal_camera < 0) {
          continue;
        }
      }

      Vec2F32 projected_points[3];
      // Loop through all the vectices and perform projection
      for (s32 k = 0; k < 3; ++k) {
        projected_points[k] = perspective_projection(transformed_vertices[k]);

        // Scale and translate projected points to the middle of the screen
        projected_points[k].x += (buffer.width / 2);
        projected_points[k].y += (buffer.height / 2);
      }

      // Calculate average depth for each face based on the vertices z value after transformation
      f32 l_average_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3;

      Triangle2F32 projected_triangle = {.points =
                                             {
                                                 {projected_points[0].x, projected_points[0].y},
                                                 {projected_points[1].x, projected_points[1].y},
                                                 {projected_points[2].x, projected_points[2].y},
                                             },
                                         .colour = current_mesh_face.colour,
                                         .average_depth = l_average_depth};
      // projected_triangle.points[k] = projected_point;

      // save projected triangle in array of triangles to be rendered
      array_push(g_triangles_to_render, projected_triangle);
    }

    // Sort the triangles to render by their average depth
    // NOTE(tijani): Bubble sort
    s32 num_triangles = array_length(g_triangles_to_render);
    for (s32 i = 0; i < num_triangles; ++i) {
      for (s32 j = i; j < num_triangles; ++j) {
        if (g_triangles_to_render[i].average_depth < g_triangles_to_render[j].average_depth) {
          Triangle2F32 temp = g_triangles_to_render[i];
          g_triangles_to_render[i] = g_triangles_to_render[j];
          g_triangles_to_render[j] = temp;
        }
      }
    }
  }

  // now draw
  {
    r_clear_colour_buffer(&buffer, 0xFF000000);
    r_draw_grid(&buffer, 0xFF444444);
    r_draw_filled_triangle(&buffer, 300, 100, 50, 400, 500, 700, 0xFF392876);

    // Loop through all projected points and render
    int triangle_count = array_length(g_triangles_to_render);

    for (u32 i = 0; i < triangle_count; ++i) {
      Triangle2F32 triangle = g_triangles_to_render[i];

      // Draw vertex points
      if (g_render_state.render_mode & RENDER_VERTEX) {
        r_draw_rect(&buffer, triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFE7104);
        r_draw_rect(&buffer, triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFE7104);
        r_draw_rect(&buffer, triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFE7104);
      }

      // Draw filled triangle
      if (g_render_state.render_mode == RENDER_DEFAULT || g_render_state.render_mode & RENDER_FILL) {
        r_draw_filled_triangle(&buffer, triangle.points[0].x, triangle.points[0].y, triangle.points[1].x,
                               triangle.points[1].y, triangle.points[2].x, triangle.points[2].y, triangle.colour);
      }

      // Draw wireframe
      if (g_render_state.render_mode & RENDER_WIREFRAME) {
        r_draw_triangle(&buffer, triangle.points[0].x, triangle.points[0].y, triangle.points[1].x, triangle.points[1].y,
                        triangle.points[2].x, triangle.points[2].y, 0xFFFFFFF);
      }
    }
    // free the triangle
    array_free(g_triangles_to_render);
  }

  // send to window
  r_copy_buffer_to_window(device_context, &buffer);

  scratch_end(scratch);
  return quit;
}

///////////////////////////////////////////////////
// Entrypoint

void entry_point() {
  window_handle = os_window_open(V2S32(1250, 900), 0, str8_lit(BUILD_TITLE));
  os_window_first_paint(window_handle);
  device_context = os_get_device_context(window_handle);
  // load_obj_file_data_from_file("data/meshes/f22.obj");
  load_cube_mesh_data();

  // equip renderer
  for (; !update();) {
    Sleep(1); // DON'T MELT DOWN THE CPU, DUFUS.
  }

  os_release_device_context(window_handle, device_context);
}