#ifndef THIRD_PARTY_INC_H
#define THIRD_PARTY_INC_H

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#define STBI_ONLY_PNG
//#define STBI_NO_STDIO
#include "stb/stb_image.h"

#define STB_SPRINTF_DECORATE(name) starlight_##name
#define STB_SPRINTF_STATIC
#define STB_SPRINTF_IMPLEMENTATION
#include "stb/stb_sprintf.h"

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "stb/stb_truetype.h"

#define KB_TEXT_SHAPE_IMPLEMENTATION
#define KB_TEXT_SHAPE_STATIC
#include "kb_text_shape/kb_text_shape.h"

#if BUILD_PROFILE 
	#if PROFILER_SUPERLUMINAL && OS_WINDOWS
		#include "Superluminal/include/PerformanceAPI.h"
		#pragma comment(lib, "../src/third_party/Superluminal/libs/PerformanceAPI_MT")
	#elif PROFILER_TRACY && OS_WINDOWS
		#define TRACY_ENABLE
		#include "tracy/include/tracy/Tracy.hpp"
		#include "tracy/include/TracyClient.cpp"
		#pragma comment(lib, "../src/third_party/tracy/libs/TracyClient")
	#else 
		#error "Profiler not recognized, are you using the correct profiler flag?"
	#endif
#endif

#endif // THIRD_PARTY_INC_H