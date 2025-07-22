#define BUILD_TITLE                        "Manic"
#define BUILD_VERSION_MAJOR                00
#define BUILD_VERSION_MINOR                00
#define BUILD_VERSION_PATCH                BUILD_SVN_REVISION
#define BUILD_RELEASE_PHASE_STRING_LITERAL "Alpha"
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

	global GameState* g_window_state = {};
	global MeshInfo* g_mesh_info = {};
	global Triangle* g_triangles_to_render = nullptr; // no-checkin
	global Camera g_camera = {};
	global Light g_light;
	global u32 g_face_count_idx = 0; // this is ugly and weird but its temporary.
	global b32 quit = 0;
	

	internal void initialize_system() {
		ProfFunction(profDebug_hotpink);

		ProfScope(init, "game memory alloc", profDebug_maroon);

		// Initialize game memory
		// @IMPROVEMENT: A pool would be good for assets and game_memory instead of the arena!
		Arena* big_daddy_arena = arena_alloc();
		g_window_state = arena_push<GameState>(big_daddy_arena, 1);
		g_window_state->game_memory = arena_alloc(); 
		g_window_state->per_frame_memory = arena_alloc();
		g_window_state->asset_memory = arena_alloc();

		// @IMPROVEMENT: It would be nice to seperate the speed which render() and update() runs.
		// This would allow us to run at the speed of the monitor without having a correlating effect
		// on the speed of update(), input(), physics(), etc.
		s32 target_monitor_hz = get_gfx_info()->monitor_refresh_rate;
		f32 game_hz = static_cast<f32>(target_monitor_hz);
		g_window_state->frame_dt = 1.f/game_hz;

		// Initialize window and paint into it
		// 16:9 aspect ratio, 675,000 pixels to process each frame.
		g_window_state->os_handle = window_open(Rng2f32(0, 0, 900, 750), str8_lit(BUILD_TITLE_STRING_LITERAL)); 
		g_window_state->window_dim = client_rect_from_window(g_window_state->os_handle);
		window_first_paint(g_window_state->os_handle);
		allocate_buffer(g_window_state->game_memory, &g_window_state->render_buffer, 
										g_window_state->window_dim.x1, g_window_state->window_dim.y1);

		// Default rendering mode 
		// @NOTE(Tijani): maybe too verbose
		g_window_state->render_buffer.render_mode = (RENDERMODE_DEFAULT | RENDERMODE_TEXTURE | RENDERMODE_CULL);

		// Initialize lights
		g_light.direction = { 0.0f, 0.0f, 1.0f };

		// Initialize camera system
		g_camera.position = { 0, 0, 0 };
		g_camera.direction = { 0, 0, 1 };
		g_camera.fov = MATH_PI / 3.0;
		g_camera.znear = 0.0f;
		g_camera.zfar = 100.0f;
		g_camera.aspect_ratio = (g_window_state->window_dim.x1 / g_window_state->window_dim.y1);
		g_camera.projection = perspective(g_camera.fov, g_camera.aspect_ratio, g_camera.znear, g_camera.zfar);

		// Initialize the resource manager.
		g_mesh_info = load_model_and_texture(g_window_state->asset_memory, str8_lit("data/meshes/crab.obj"), str8_lit("data/textures/crab.png"));
		g_mesh_info->scale = { 1.0, 1.0, 1.0};
		g_mesh_info->colour = 0xFFFFFFFF;

		#if BUILD_DEBUG_VERY_NOISY
			// @IMPROVEMENT: provide this information in with text overlays when text rendering is a thing!
			dump_mesh_info(g_mesh_info);
		#endif

		g_triangles_to_render = arena_push<Triangle>(g_window_state->per_frame_memory, g_mesh_info->faces_count);
	}

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

						// Toggle render modes
						case(Key_W): { g_window_state->render_buffer.render_mode ^= RENDERMODE_WIREFRAME; } break;
						case(Key_F): { g_window_state->render_buffer.render_mode ^= RENDERMODE_FILL; } break;
						case(Key_V): { g_window_state->render_buffer.render_mode ^= RENDERMODE_VERTEXPOINTS; } break;
						case(Key_T): { g_window_state->render_buffer.render_mode ^= RENDERMODE_TEXTURE; } break;
						case(Key_C): { g_window_state->render_buffer.render_mode ^= RENDERMODE_CULL; } break;

						case(Key_Up): {
							g_mesh_info->rotate.x -= 0.5f;
						} break;

						case(Key_Down): {
							g_mesh_info->rotate.x += 0.5;
						} break;

						case(Key_Left): {
							g_mesh_info->rotate.y += 0.5;
						} break;

						case(Key_Right): {
							g_mesh_info->rotate.y -= 0.5;
						} break;

						case(Key_A): {
							g_mesh_info->rotate.z -= 0.5;
						} break;

						case(Key_O): {
							g_mesh_info->rotate.z += 0.5;
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

	internal void update() {
		ProfFunction(profDebug_orangered);

		g_face_count_idx = 0;
		g_mesh_info->rotate.x += 0.000;
    g_mesh_info->rotate.y += 0.000;
		g_mesh_info->rotate.z += 0.000;
		g_mesh_info->translate.z = 5.0;

		// Change camera position per animation frame
		g_camera.position.x += 0.007;
		g_camera.position.y += 0.007;

		Vec3 target = {0, 0, 4.0 }; // @TEMP
		Vec3 up = { 0, 1, 0 }; // @TEMP??

		Matrix view 			= matrix_lookat(g_camera.position, target, up);
		Matrix scale     	= matrix_scale(g_mesh_info->scale.x, g_mesh_info->scale.y, g_mesh_info->scale.z);
		Matrix translate 	= matrix_translate(g_mesh_info->translate.x, g_mesh_info->translate.y, g_mesh_info->translate.z);
		Matrix rotate_x 	= matrix_rotate_x(g_mesh_info->rotate.x);
		Matrix rotate_y 	= matrix_rotate_y(g_mesh_info->rotate.y);
		Matrix rotate_z 	= matrix_rotate_z(g_mesh_info->rotate.z);

		s32 fc = g_mesh_info->faces_count;
		for(s32 i = 0; i < fc; i++) {
			Face current_face = g_mesh_info->faces[i];
			Vec3 face_vertices[3];

			face_vertices[0] = g_mesh_info->vertices[current_face.vertex_idx[0] - 1];
			face_vertices[1] = g_mesh_info->vertices[current_face.vertex_idx[1] - 1];
			face_vertices[2] = g_mesh_info->vertices[current_face.vertex_idx[2] - 1];

			Vec4 transformed_vertices[3];

			// Loop through all vertices in the current face and apply transformation
			for(s32 j = 0; j < 3; j++) {
				Vec4 transformed_vertex = vec4_from_vec3(face_vertices[j]);
				Matrix world_matrix = matrix_identity();

				// Order matters in how things are done, not respecting that means things are in weird places.
				world_matrix = scale     * world_matrix;
				world_matrix = rotate_z  * world_matrix;
				world_matrix = rotate_y  * world_matrix;
				world_matrix = rotate_x  * world_matrix;
				world_matrix = translate * world_matrix;

				// Multiply world matrix by original vector
				transformed_vertex = matrix_multiply_vec4(world_matrix, transformed_vertex);

				// Multiply the view matrix by the vector to transform the scene to camera space
				//transformed_vertex = matrix_multiply_vec4(view, transformed_vertex);

				transformed_vertices[j] = transformed_vertex;
			}

			// Triangles are clocwise
			Vec3 a = vec3_from_vec4(transformed_vertices[0]);
			Vec3 b = vec3_from_vec4(transformed_vertices[1]);
			Vec3 c = vec3_from_vec4(transformed_vertices[2]);

			Vec3 ab = b - a;
			Vec3 ac = c - a;
			ab = vec3_normalize(ab);
			ac = vec3_normalize(ac);

			// !! Culling !!
			// Computer face normal using cross product to find perpendicular
			Vec3 face_normal = vec3_cross_product(ac, ab);
			face_normal = vec3_normalize(face_normal);

			// Find the vector between points in the triangle and camera origin
			Vec3 origin = {0, 0, 0};
			Vec3 camera_ray = origin - a;

			// If face normal (dot product) is aligned with camera ray, draw, if not cull.
			f32 dot_normal_camera = vec3_dot_product(face_normal, camera_ray);

			if(g_window_state->render_buffer.render_mode & RENDERMODE_CULL) {
				if(dot_normal_camera < 0) {
					continue;
				}
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

			// Calculate shading density based on how aligned the face normal and the light ray are.
			f32 intensity = -vec3_dot_product(face_normal, g_light.direction);
			u32 triangle_colour = light_intensity(g_mesh_info->colour, intensity);

			Triangle projected_triangle = {
				{
					{ projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w },
					{ projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w },
					{ projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w },
				}, 
				{
					{ g_mesh_info->texture_coords[current_face.texture_idx[0] - 1].u, g_mesh_info->texture_coords[current_face.texture_idx[0] - 1].v },
					{ g_mesh_info->texture_coords[current_face.texture_idx[1] - 1].u, g_mesh_info->texture_coords[current_face.texture_idx[1] - 1].v },
					{ g_mesh_info->texture_coords[current_face.texture_idx[2] - 1].u, g_mesh_info->texture_coords[current_face.texture_idx[2] - 1].v },
				},
				triangle_colour
			};

			g_triangles_to_render[g_face_count_idx++] = projected_triangle;
		}
	}

	internal void render() {
		ProfFrameMark;
		ProfFunction(profDebug_red);
		clear_colour_buffer(&g_window_state->render_buffer, 0xFF2C2C2C);
		clear_z_buffer(&g_window_state->render_buffer);
		
		// Batch process these
		for(u32 i = 0; i < g_face_count_idx; i++) {
			Triangle triangle = g_triangles_to_render[i];
			// Draw vertex points
			if(g_window_state->render_buffer.render_mode & RENDERMODE_VERTEXPOINTS){
				draw_rect(&g_window_state->render_buffer, triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFE7104);
				draw_rect(&g_window_state->render_buffer, triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFE7104);
				draw_rect(&g_window_state->render_buffer, triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFE7104);
			}
		}

		for(u32 i = 0; i < g_face_count_idx; i++) {
			Triangle triangle = g_triangles_to_render[i];

			// Draw filled triangles
			if(g_window_state->render_buffer.render_mode == RENDERMODE_DEFAULT || g_window_state->render_buffer.render_mode & RENDERMODE_FILL) {
				//if(g_window_state->render_buffer.render_mode & RENDERMODE_FILL) {
				draw_filled_triangle(&g_window_state->render_buffer, 
														 triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, // Vertex A
														 triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, // Vertex B
														 triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, // Vertex C
														 triangle.colour);
			}
		}

		for(u32 i = 0; i < g_face_count_idx; i++) {
			Triangle triangle = g_triangles_to_render[i];

			// Draw wireframe
			if(g_window_state->render_buffer.render_mode & RENDERMODE_WIREFRAME) {
				draw_triangle(&g_window_state->render_buffer, 
											triangle.points[0].x, triangle.points[0].y,		 // Vertex A
											triangle.points[1].x, triangle.points[1].y,    // Vertex B
											triangle.points[2].x, triangle.points[2].y,    // Vertex C
											0xFFFFFFF);
			}
		}

		for(u32 i = 0; i < g_face_count_idx; i++) {
			Triangle triangle = g_triangles_to_render[i];

			// Draw textured triangle
			if (g_window_state->render_buffer.render_mode & RENDERMODE_TEXTURE) {
				draw_textured_triangle(&g_window_state->render_buffer, 
					triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texture_coords[0].u, triangle.texture_coords[0].v, // Vertex A
					triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texture_coords[1].u, triangle.texture_coords[1].v, // Vertex B
					triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texture_coords[2].u, triangle.texture_coords[2].v, // Vertex C
					g_mesh_info->texture_data, g_mesh_info->texture_width, g_mesh_info->texture_height);
			}
		}

		copy_buffer_to_window(g_window_state->os_handle, &g_window_state->render_buffer);
	}

	internal void main_loop() {
		initialize_system();

		u64 previous_frame_time = get_high_res_time();
    u64 target_frame_time = (u64)(g_window_state->frame_dt * (f32)Million(1)); // Convert to microseconds

		while(!quit) {
			u64 frame_start_time = get_high_res_time();
			process_input();
			update();
			render();

			// Frame stats
			u64 frame_end_time = get_high_res_time();
			u64 frame_work_time = frame_end_time - frame_start_time;
			if(frame_work_time < target_frame_time) {
				u64 sleep_time = target_frame_time - frame_work_time;
				// @IMPROVE/@INVESTIGATE: using timeBeginPeriod to set the scheduler timer lower,
				// which in turn resolves the latency issue with Sleep().
				// Need to do more research into if that actually works!
				Sleep((DWORD)(sleep_time / 1000));
			}
		}
	}
}
