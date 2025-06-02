#ifndef THIRD_PARTY_INC_H
#define THIRD_PARTY_INC_H

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_NO_STDIO
#include "stb/stb_image.h"

#define STB_SPRINTF_DECORATE(name) starlight_##name
#define STB_SPRINTF_STATIC
#define STB_SPRINTF_IMPLEMENTATION
#include "stb/stb_sprintf.h"

#if BUILD_PROFILE && PROFILE_SUPERLUMINAL && OS_WINDOWS
	#include "Superluminal/include/PerformanceAPI.h"
	#pragma comment(lib, "../src/third_party/Superluminal/libs/PerformanceAPI_MTd")
#endif

#endif // THIRD_PARTY_INC_H