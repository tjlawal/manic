#pragma once

namespace Starlight {
	namespace Foundation {

		//union Vertex {
		//	struct {
		//		f32 x;
		//		f32 y;
		//		f32 z;
		//	};

		//	f32 v[3];
		//};

		struct Face {
			s32 vertex_idx[3];
			s32 texture_idx[3];
			s32 normal_idx[3];
			u32 colour;
		};

		union TextureCoord {
			struct {
				f32 u;
				f32 v;
			};

			f32 uv[2];
		};

		struct Triangle {
			Vec4 points[3];
			//TextureCoord texture_coords[3];
			u32 colour;
		};

		// Calculate the barycentric weights of alpha, beta and gamma for point p;
		// REVISE: This function is slow, it could definitely be faster just have to find out how!!
		FORCE_INLINE internal Vec3 barycentric_weights(Vec2f a, Vec2f b, Vec2f c, Vec2f p) {
			// Find the vectors between the vertices ABC and point p
			Vec2f ac = c - a;
			Vec2f ab = b - a;
			Vec2f pc = c - p;
			Vec2f pb = b - p;
			Vec2f ap = p - a;

			// Area of the parallelogram (triangle ABC) using cross product
			f32 area_parallelogram_abc = ((ac.x * ab.y) - (ac.y * ab.x)); // || AC x AB ||

			// Alpha is area of the parallelogram [PBC] over the area of the full parallelogram [ABC]
			f32 alpha = ((pc.x * pb.y) - (pc.y * pb.x)) / area_parallelogram_abc;

			// Beta is area of the parallelogram [APC] over the area of the
			// full parallelogram [ABC]
			f32 beta = ((ac.x * ap.y) - (ac.y * ap.x)) / area_parallelogram_abc;
			f32 gamma = 1.0 - alpha - beta;
			Vec3 weights = {alpha, beta, gamma};
			return weights;
		}
	}
}
