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
//HDC device_context;
global OS_Handle window_handle = {0};
global Triangle2F32 *g_triangles_to_render = NULL;
global RenderState *g_render_state = {.is_wireframe_vertex = false,
                                      .is_wireframes = true,
                                      .is_fill_colour = false,
                                      .is_fill_wireframes = false,
                                      .render_mode = Render_FillWireframes,
                                      .culling_mode = Cull_BackFace}

// NOTE(tijani): TEMP, quick and dirty visualization debug code. make better and put in appropriate place
Vec3F32
get_triangle_center(Vec3F32 a, Vec3F32 b, Vec3F32 c) {
  Vec3F32 center = {.x = (a.x + b.x + c.x) / 3.0f, .y = (a.y + b.y + c.y) / 3.0f, .z = (a.z + b.z + c.z) / 3.0f};
  return center;
}

internal Vec3F32 normalize_vector(Vec3F32 v) {
  f32 length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
  if (length != 0.0f) {
    v.x /= length;
    v.y /= length;
    v.z /= length;
  }
  return v;
}

////////////////////////////////////////////////////////////////////

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

					// NOTE(tijani): Toggle render modes here
          case (OS_Key_1): {
            if (g_render_state.is_wireframe_vertex) {
              g_render_state = Render_WireFramesVertex;
            }
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

      // Loop through all the vertices of the current face and apply transformation
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
      {
        // @NOTE: triangles are clockwise.
        // find vectors (b - a) and (c -a)
        Vec3F32 a = transformed_vertices[0];
        Vec3F32 b = transformed_vertices[1];
        Vec3F32 c = transformed_vertices[2];

        Vec3F32 ab = vec3_sub(b, a);
        Vec3F32 ac = vec3_sub(c, a);

        // Compute the face normal using cross product to find perpendicular.
        Vec3F32 normal = vec3_cross(ab, ac);

        // Find vector between point in the triangle and the camera origin
        Vec3F32 camera_ray = vec3_sub(camera_position, a);

        // if face normal (dot_product) is aligned with camera ray, draw if not cull (dont draw).
        f32 dot_normal_camera = vec3_dot(normal, camera_ray);

        if (dot_normal_camera < 0) {
          continue; // dont render if looking away  from the camera, remember we are a right handed coordinate system.
        }

        // NOTE(tijani): Quick and dirty visual debugging code, calculate normal vector visualization points
        //        const f32 NORMAL_LINE_LENGTH = 4.0f;
        //        Vec3F32 face_center =
        //            get_triangle_center(transformed_vertices[0], transformed_vertices[1], transformed_vertices[2]);

        //       Vec3F32 normal_end = {.x = face_center.x + normal.x * NORMAL_LINE_LENGTH,
        //                             .y = face_center.y + normal.y * NORMAL_LINE_LENGTH,
        //                             .z = face_center.z + normal.z * NORMAL_LINE_LENGTH};

        Triangle2F32 projected_triangle;

        // Loop through all the vectices and perform projection
        for (s32 k = 0; k < 3; ++k) {
          Vec2F32 projected_point = perspective_projection(transformed_vertices[k]);

          // Scale and translate projected points to the middle of the screen
          projected_point.x += (buffer.width / 2);
          projected_point.y += (buffer.height / 2);

          projected_triangle.points[k] = projected_point;
        }

        // NOTE(tijani): Quick and dirty visual debugging code, make more mature.
        // Project normal visualization points
        //       Vec2F32 center_2d = perspective_projection(face_center);
        //       Vec2F32 normal_end_2d = perspective_projection(normal_end);

        //       // Transform normal points to screen space
        //       center_2d.x += (buffer.width / 2);
        //       center_2d.y += (buffer.height / 2);
        //       normal_end_2d.x += (buffer.width / 2);
        //       normal_end_2d.y += (buffer.height / 2);

        //       projected_triangle.normal_start = center_2d;
        //       projected_triangle.normal_end = normal_end_2d;

        // save projected triangle in array of triangles to be rendered
        array_push(g_triangles_to_render, projected_triangle);
      }
    }
  }

  // now draw
  {
    r_clear_colour_buffer(&buffer, 0xFF000000);
    r_draw_grid(&buffer, 0xFF444444);
    // r_draw_filled_triangle(&buffer, 300, 100, 50, 400, 500, 700, 0xFF392876);

    // Loop through all projected points and render
    int triangle_count = array_length(g_triangles_to_render);

    for (u32 i = 0; i < triangle_count; ++i) {
      Triangle2F32 triangle = g_triangles_to_render[i];

      if (g_render_mode == Render_WireFramesVertex) {
        // Draw vertex points
        r_draw_rect(&buffer, triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFF0000);
        r_draw_rect(&buffer, triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFF0000);
        r_draw_rect(&buffer, triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFF0000);
      }

      // Draw unfilled triangle
      r_draw_filled_triangle(&buffer, triangle.points[0].x, triangle.points[0].y, triangle.points[1].x,
                             triangle.points[1].y, triangle.points[2].x, triangle.points[2].y, 0xFFFFFF);

      r_draw_triangle(&buffer, triangle.points[0].x, triangle.points[0].y, triangle.points[1].x, triangle.points[1].y,
                      triangle.points[2].x, triangle.points[2].y, 0xFF000000);

      // NOTE(tijani): Visual debugging code.
      // draw normal vector
      // r_draw_line_dda(&buffer, triangle.normal_start.x, triangle.normal_start.y, triangle.normal_end.x,
      //                triangle.normal_end.y, 0xFFFF0000);
      // r_draw_line_dda(&buffer, triangle.normal_start.x, triangle.normal_start.y, triangle.normal_end.x,
      // triangle.normal_end.y, 0xFFFF0000);

      // Draw normal vector base point
      r_draw_rect(&buffer, triangle.normal_start.x - 1, triangle.normal_start.y - 1, 2, 2, 0xFFFFFFFF);
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
  load_obj_file_data_from_file("data/meshes/f22.obj");

  // equip renderer
  for (; !update();) {
    Sleep(1); // DON'T MELT DOWN THE CPU, DUFUS.
  }

  os_release_device_context(window_handle, device_context);
}