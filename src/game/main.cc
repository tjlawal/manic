#define BUILD_TITLE                        "Manic"
#define BUILD_VERSION_MAJOR                00
#define BUILD_VERSION_MINOR                00
#define BUILD_VERSION_PATCH                BUILD_SVN_REVISION
#define BUILD_RELEASE_PHASE_STRING_LITERAL "Pre-Alpha"
#define RENDER_SW													 1

// Includes, order is important here.
#include "foundation/context_switch.h"
#include "third_party/third_party.h"

// [.h]
#include "foundation/foundation.h"
#include "platform/platform.h"
#include "render/render.h"
#include "draw/draw.h"
#include "resource_manager/resource_manager.h"
#include "game/manic.h"

// [.c]
#include "foundation/foundation.cc"
#include "platform/platform.cc"
#include "render/render.cc"
#include "draw/draw.cc"
#include "resource_manager/resource_manager.cc"

using namespace Starlight::Foundation;
using namespace Starlight::Platform;
using namespace Starlight::Platform::Gfx;
using namespace Starlight::Render;
using namespace Starlight::Draw;
using namespace Starlight::ResourceManager;


namespace Starlight {

	s64 g_perf_frequency;
	global GameState* g_window_state = {};
	global MeshInfo* g_mesh_info = {};
	global Triangle* g_triangles_to_render = nullptr; // no-checkin
	global Camera g_camera = {};
	global Light g_light;
	global u32 g_face_count_idx = 0; // this is ugly and weird but its temporary.
	global b32 quit = 0;
	

	internal void process_input() {
		ProfFunction(profDebug_lightgray);
		Temp scratch = scratch_begin(0, 0);

		EventList events = get_events(scratch.arena, 0);
		for(Event *event = events.first; event != 0; event = event->next) {
			switch(event->event_kind) {
				case(EventKind_WindowClose): {
					quit = 1;
				} break;

				case (EventKind_Press): {
					switch(event->key) {
						case(Key_Q): {
							quit = 1;
						} break;
						
						case(Key_Up): {
							g_mesh_info->rotate.z -= 0.5f;
						} break;

						case(Key_Down): {
							g_mesh_info->rotate.z += 0.5;
						} break;

						case(Key_Left): {
							g_mesh_info->rotate.y -= 0.5;
						} break;

						case(Key_Right): {
							g_mesh_info->rotate.y += 0.5;
						} break;


						default:
							break;
					} break;
				}

				default:
					break;
			}
		}

		scratch_end(scratch);
	}

	internal void initialize_system() {
		ProfFunction(profDebug_hotpink);

		ProfScope(init, "game memory alloc", profDebug_maroon);

		// Initialize game memory
		// @IMPROVEMENT: A pool would be good for asset stuff instead of the arena!
		Arena* big_daddy_arena = arena_alloc();
		g_window_state = arena_push<GameState>(big_daddy_arena, 1);
		g_window_state->game_memory = arena_alloc(); 
		g_window_state->per_frame_memory = arena_alloc();
		g_window_state->asset_memory = arena_alloc();

		// Get monitor frame rate stuff
		s32 monitor_hz = get_gfx_info()->monitor_refresh_rate;
		f32 game_hz = static_cast<f32>(monitor_hz/2);
		g_window_state->frame_dt = 1.f/game_hz;

		// Initialize window and paint into it
		// 16:9 aspect ratio, 518,400 pixels to process each frame.
		g_window_state->os_handle = window_open(Rng2f32(0, 0, 900, 800), str8_lit(BUILD_TITLE_STRING_LITERAL)); 
		g_window_state->window_dim = client_rect_from_window(g_window_state->os_handle);
		window_first_paint(g_window_state->os_handle);
		allocate_buffer(g_window_state->game_memory, &g_window_state->render_buffer, 
										g_window_state->window_dim.x1, g_window_state->window_dim.y1);

		// Initialize lights
		g_light.direction.x = 0.0f;
		g_light.direction.y = 0.0f;
		g_light.direction.z = 1.0f;

		//// Initialize camera system
		g_camera.fov = MATH_PI / 3.0;
		g_camera.znear = 50.0;
		g_camera.zfar = 100.0;
		g_camera.aspect_ratio = g_window_state->window_dim.x1 / g_window_state->window_dim.y1;
		g_camera.projection = perspective(g_camera.fov, g_camera.aspect_ratio, g_camera.znear, g_camera.zfar);

		// Initialize the resource manager.
		g_mesh_info = load_model(g_window_state->asset_memory, str8_lit("data/meshes/f117.obj"));
		g_mesh_info->scale = {1.0, 1.0, 1.0};

		#if BUILD_DEBUG_VERY_NOISY
			dump_mesh_info(g_mesh_info); // @IMPROVEMENT: profived this information in with text overlays when text rendering is a thing!
		#endif

		g_triangles_to_render = arena_push<Triangle>(g_window_state->per_frame_memory, g_mesh_info->faces_count);
	}

	internal void update() {
		ProfFunction(profDebug_orangered);

		g_face_count_idx = 0;
		g_mesh_info->rotate.x += 0.01;
		//g_mesh_info->rotate.y += 0.01;
		//g_mesh_info->rotate.z += 0.001;
		g_mesh_info->translate.z = 5.0;

		Matrix scale     = matrix_scale(g_mesh_info->scale.x, g_mesh_info->scale.y, g_mesh_info->scale.z);
		Matrix translate = matrix_translate(g_mesh_info->translate.x, g_mesh_info->translate.y, g_mesh_info->translate.z);
		Matrix rotate_x = matrix_rotate_x(g_mesh_info->rotate.x);
		Matrix rotate_y = matrix_rotate_y(g_mesh_info->rotate.y);
		Matrix rotate_z = matrix_rotate_z(g_mesh_info->rotate.z);

		s32 fc = g_mesh_info->faces_count;
		for(s32 i = 0; i < fc; i++) {
			Face current_face = g_mesh_info->faces[i];
			Vec3 face_vertices[3];

			face_vertices[0] = g_mesh_info->vertices[current_face.vertex_idx[0] - 1];
			face_vertices[1] = g_mesh_info->vertices[current_face.vertex_idx[1] - 1];
			face_vertices[2] = g_mesh_info->vertices[current_face.vertex_idx[2] - 1];

			Vec4 transformed_vertices[3];
			// Loop through all vertices in teh current face and apply transformation
			for(s32 j = 0; j < 3; j++) {
				Vec4 transformed_vertex = vec4_from_vec3(face_vertices[j]);
				Matrix world_matrix = matrix_identity();

				// Order matters in how things are done, not respecting that means things are in weird places.
				world_matrix = scale * world_matrix;
				world_matrix = rotate_x * world_matrix;
				world_matrix = rotate_y * world_matrix;
				world_matrix = rotate_z * world_matrix;
				world_matrix = translate * world_matrix;

				// Multiply world matrix by original vector
				transformed_vertex = matrix_multiply_vec4(world_matrix, transformed_vertex);
				transformed_vertices[j] = transformed_vertex;
			}

			// Triangles are clocwise
			Vec3 a = vec3_from_vec4(transformed_vertices[0]);
			Vec3 b = vec3_from_vec4(transformed_vertices[1]);
			Vec3 c = vec3_from_vec4(transformed_vertices[2]);

			Vec3 ab = b - a;
			Vec3 ac = c - a;
			vec3_normalize(ab);
			vec3_normalize(ac);

			// Computer face normal using cross product to find perpendicular
			Vec3 normal = vec3_cross_product(ab, ac);
			vec3_normalize(normal);

			// Find the vector between points in the triangle and camera origin
			Vec3 camera_ray = g_camera.position - a;

			// If face normal (dot product) is aligned with camera ray, draw, if not cull.
			f32 dot_normal_camera = vec3_dot_product(normal, camera_ray);
			if(dot_normal_camera < 0) {
				continue;
			}

			Vec4 projected_points[3];
			for(s32 k = 0; k < 3; k++) {
				// Project current vertex
				projected_points[k] = mat4f32_mul_projection(g_camera.projection, transformed_vertices[k]); 

				// scale viewport
				projected_points[k].x *= (g_window_state->render_buffer.sw.width / 2.0);
				projected_points[k].y *= (g_window_state->render_buffer.sw.height / 2.0);

				// translate projectd points to the middle of the screen
				projected_points[k].x += (g_window_state->render_buffer.sw.width / 2.0);
				projected_points[k].y += (g_window_state->render_buffer.sw.height / 2.0);
			}

			// There's a lighting bug but cant figure it out yet
			// @BUG!!
			// calculate light intensity based on the alignment of the face normal and the light ray
			f32 light_intensity_factor = -vec3_dot_product(normal, g_light.direction);
			light_intensity_factor *= 5.0f; // Boost the light temporarily
			// Calculate the triangle color based on the light angle
			u32 triangle_colour = light_intensity(0xffffffff, light_intensity_factor); 

			Triangle projected_triangle = {{
				{ projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w },
				{ projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w },
				{ projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w },
			}, triangle_colour
			};

			g_triangles_to_render[g_face_count_idx++] = projected_triangle;
		}

		//u64 end_time_us = get_high_res_time();
		//u64 frame_time_us = end_time_us - begin_time_us;

		//printf("%lld FPS/ms\n", frame_time_us);

	}

	internal void render() {
		ProfFrameMark;
		ProfFunction(profDebug_red);
		clear_colour_buffer(&g_window_state->render_buffer, 0xFF2C2C2C);
		clear_z_buffer(&g_window_state->render_buffer);
		
		//s32 triangle_count = array_length(g_triangles_to_render);
		for(u32 i = 0; i < g_face_count_idx; i++) {
			Triangle triangle = g_triangles_to_render[i];
			//draw_rect(&g_window_state->render_buffer, triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFE7104);
			//draw_rect(&g_window_state->render_buffer, triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFE7104);
			//draw_rect(&g_window_state->render_buffer, triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFE7104);


			draw_filled_triangle(&g_window_state->render_buffer, 
													 triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, // Vertex A
													 triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, // Vertex B
													 triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, // Vertex C
													 triangle.colour);

			draw_triangle(&g_window_state->render_buffer, 
										triangle.points[0].x, triangle.points[0].y,		 // Vertex A
										triangle.points[1].x, triangle.points[1].y,    // Vertex B
										triangle.points[2].x, triangle.points[2].y,    // Vertex C
										0xFFFFFFF);
		}


		copy_buffer_to_window(g_window_state->os_handle, &g_window_state->render_buffer);
	}

	internal void main_loop() {
		initialize_system();

		while(!quit) {
			process_input();
			update();
			render();

			//sleep(16); // This is sucky
		}
	}
}

// WIP

//	internal void main_loop() {
//    initialize_system();

//    u64 last_frame_time = get_high_res_time();
//    f32 fps_timer = 0.0f;
//    u32 frame_count = 0;
//    f32 average_fps = 0.0f;

//		s32 monitor_hz = get_gfx_info()->monitor_refresh_rate;
//		f32 game_hz = static_cast<f32>(monitor_hz/2);
//		g_window_state->frame_dt = 1.f/game_hz;

//    while(!quit) {
//			u64 frame_start_time = get_high_res_time();
        
//			// Do all frame work
//			process_input();
//			update();
//			render();
        
//			u64 frame_end_time = get_high_res_time();
        
//			// Calculate frame timing
//			u64 frame_time_us = frame_end_time - frame_start_time;
//			f32 frame_time_ms = frame_time_us / 1000.0f;
//			f32 current_fps = 1000.0f / frame_time_ms;
        
//			// Update FPS averaging (update every second)
//			fps_timer += frame_time_ms;
//			frame_count++;
        
//			if(fps_timer >= 1000.0f) { // Every 1000ms (1 second)
//				average_fps = frame_count / (fps_timer / 1000.0f);
//				printf("FPS: %.1f | Frame Time: %.2f ms | Target: %.1f FPS\n",  average_fps, frame_time_ms, 1.0f / g_window_state->frame_dt);
            
//				// Reset counters
//				fps_timer = 0.0f;
//				frame_count = 0;
//			}
        
//			last_frame_time = frame_end_time;
//    }
//	}
//}
