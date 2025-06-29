#define BUILD_TITLE                        "Manic"
#define BUILD_VERSION_MAJOR                00
#define BUILD_VERSION_MINOR                00
#define BUILD_VERSION_PATCH                BUILD_SVN_REVISION
#define BUILD_RELEASE_PHASE_STRING_LITERAL "Pre-Alpha"

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

						case(Key_A): {
							g_mesh_info->rotate.y += 0.1;
						} break;

						case(Key_O): {
							g_mesh_info->rotate.x += 0.1;
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

		Arena* big_daddy_arena = arena_alloc();
		g_window_state = arena_push<GameState>(big_daddy_arena, 1);
		g_window_state->game_memory = arena_alloc(); 
		g_window_state->per_frame_memory = arena_alloc();
		g_window_state->asset_memory = arena_alloc();


		// Initialize window and paint into it
		// 16:9 aspect ratio, 518,400 pixels to process each frame.
		g_window_state->os_handle = window_open(Rng2f32(0, 0, 1920, 1080), str8_lit(BUILD_TITLE_STRING_LITERAL)); 
		g_window_state->window_dim = rect_from_window(g_window_state->os_handle);
		window_first_paint(g_window_state->os_handle);
		allocate_buffer(g_window_state->game_memory, &g_window_state->render_buffer, 
												g_window_state->window_dim.x1, g_window_state->window_dim.y1);

		// Initialize lights
		g_light.direction.z = 1;

		//// Initialize camera system
		g_camera.fov = MATH_PI / 3.0;
		g_camera.znear = 1.0;
		g_camera.zfar = 100.0;
		g_camera.aspect_ratio = g_window_state->window_dim.x1 / g_window_state->window_dim.y1;
		g_camera.projection = perspective_project(g_camera.fov, g_camera.aspect_ratio, g_camera.znear, g_camera.zfar);

		// Initialize the resource manager.
		g_mesh_info = load_model(g_window_state->asset_memory, str8_lit("data/meshes/crab.obj"));
		g_mesh_info->scale = {1.0, 1.0, 1.0};

		#if BUILD_DEBUG_VERY_NOISY
		dump_mesh_info(g_mesh_info);
		#endif

		g_triangles_to_render = arena_push<Triangle>(g_window_state->per_frame_memory, g_mesh_info->faces_count);
	}

	internal void update() {
		ProfFunction(profDebug_orangered);

		g_face_count_idx = 0;
		//g_mesh_info->rotate.y += 0.051;
		//g_mesh_info->rotate.x += 0.056;
		//g_mesh_info->rotate.z += 0.001;
		g_mesh_info->translate.z = 5.0;
		//g_mesh_info->translate.y = 2.5;

		Matrix4 scale = Matrix4::scale(g_mesh_info->scale.x, g_mesh_info->scale.y, g_mesh_info->scale.z);
		Matrix4 translate = Matrix4::translate(g_mesh_info->translate.x, g_mesh_info->translate.y, g_mesh_info->translate.z);
		Matrix4 rotate_x = Matrix4::rotate_x(g_mesh_info->rotate.x);
		Matrix4 rotate_y = Matrix4::rotate_y(g_mesh_info->rotate.y);
		Matrix4 rotate_z = Matrix4::rotate_z(g_mesh_info->rotate.z);

		s32 fc = g_mesh_info->faces_count;
		for(s32 i = 0; i < fc; i++) {
			Face current_face = g_mesh_info->faces[i];
			Vec3 face_vertices[3];

			face_vertices[0] = g_mesh_info->vertices[current_face.vertex_idx[0]];
			face_vertices[1] = g_mesh_info->vertices[current_face.vertex_idx[1]];
			face_vertices[2] = g_mesh_info->vertices[current_face.vertex_idx[2]];

			Vec4 transformed_vertices[3];
			// Loop through all vertices in teh current face and apply transformation
			for(s32 j = 0; j < 3; j++) {
				Vec4 transformed_vertex = Vec4::vec4_from_vec3(face_vertices[j]);
				Matrix4 world_matrix = Matrix4::identity();

				// Order matters in how things are done, not respecting that means things are in weird places.
				world_matrix = Matrix4::mat4f32_mul_mat4f32(scale, world_matrix);
				world_matrix = Matrix4::mat4f32_mul_mat4f32(rotate_x, world_matrix);
				world_matrix = Matrix4::mat4f32_mul_mat4f32(rotate_y, world_matrix);
				world_matrix = Matrix4::mat4f32_mul_mat4f32(rotate_z, world_matrix);
				world_matrix = Matrix4::mat4f32_mul_mat4f32(translate, world_matrix);

				// Multiply world matrix by original vector
				transformed_vertex = Matrix4::mat4f32_mul_vec4(world_matrix, transformed_vertex);
				transformed_vertices[j] = transformed_vertex;
			}

			// Triangles are clocwise
			Vec3 a = Vec3::vec3_from_vec4(transformed_vertices[0]);
			Vec3 b = Vec3::vec3_from_vec4(transformed_vertices[1]);
			Vec3 c = Vec3::vec3_from_vec4(transformed_vertices[2]);

			Vec3 ab = b - a;
			Vec3 ac = c - a;
			normalize(&ab);
			normalize(&ac);

			// Computer face normal using cross product to find perpendicular
			Vec3 normal = cross(ab, ac);
			normalize(&normal);

			// Find the vector between points in the triangle and camera origin
			Vec3 camera_ray = g_camera.position - a;

			// If face normal (dot product) is aligned with camera ray, draw, if not cull.
			f32 dot_normal_camera = dot(normal, camera_ray);
			if(dot_normal_camera < 0) {
				continue;
			}

			Vec4 projected_points[3];
			for(s32 k = 0; k < 3; k++) {
				// Project current vertex
				projected_points[k] = Matrix4::mat4f32_mul_projection(g_camera.projection, transformed_vertices[k]); 

				// scale viewport
				projected_points[k].x *= (g_window_state->render_buffer.sw.width / 2.0);
				projected_points[k].y *= (g_window_state->render_buffer.sw.height / 2.0);

				// translate projectd points to the middle of the screen
				projected_points[k].x += (g_window_state->render_buffer.sw.width / 2.0);
				projected_points[k].y += (g_window_state->render_buffer.sw.height / 2.0);
			}

			// calculate light intensity based on the alignment of the face normal and the light ray
			f32 light_intensity_factor = -dot(normal, g_light.direction);
			u32 triangle_colour = light_intensity(0xFFFFFFFF, light_intensity_factor);

			Triangle projected_triangle = {{
					{ projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w },
					{ projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w },
					{ projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w },
				}, triangle_colour
			};

			g_triangles_to_render[g_face_count_idx++] = projected_triangle;
		}

	}

	internal void render() {
		ProfFunction(profDebug_red);
		clear_colour_buffer(&g_window_state->render_buffer, 0x00000019);

		//s32 triangle_count = array_length(g_triangles_to_render);
		for(u32 i = 0; i < g_face_count_idx; i++) {
			Triangle triangle = g_triangles_to_render[i];
			draw_rect(&g_window_state->render_buffer, triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFE7104);
			draw_rect(&g_window_state->render_buffer, triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFE7104);
			draw_rect(&g_window_state->render_buffer, triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFE7104);


			//draw_filled_triangle(
			//	&g_window_state->render_buffer, 
			//	triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, // Vertex A
			//	triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, // Vertex B
			//	triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, // Vertex C
			//	triangle.colour);
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


