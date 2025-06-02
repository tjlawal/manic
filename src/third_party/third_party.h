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


#endif // THIRD_PARTY_INC_H