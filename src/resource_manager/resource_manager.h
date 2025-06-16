#pragma once

// The general idea behind this is to have a unified API that handles parsing out information from different model file formats.
// The parser layer handles the general things that is common among parsers then the folders specific to different file formats handles the actual parsing aspect
// of things.
// Not sure if this is a good way of doing things with respect to games but that's what I have right now - Tijani 06/14/2026

#pragma once

#include "parsers/parser.h"
#include "parsers/wavefrontobj/wavefrontobj.h"

namespace Starlight {
	namespace ResourceManager {
		
		internal void* load_model(Arena* arena, string8 file);
	}
}

/*
Some notes about this

Currently this only caters about loading one 3d model at application startup. Ideally this would be done asynchronously in a background thread, i.e. Asset Streaming., with the ability to load different types of models, animation, etc.

Another limitiation is that it loads one file for the application lifetime. Ideally, it would 

*/
//internal void resource_manager_init(void);
