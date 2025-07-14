/******************************************************************************
* General Info (07/13/2025):
*
* - All files that are specific to the ResourceManager should be included here
* 	ONCE! This system uses a unity build, and order should be in which files 
*		are included should be paid attention too.
* - The general idea behind this is to have a unified API that handles parsing 
* 	out information from different model file formats.
* - The parsing layer handles things common to parsing irrespective of format.
* - This only handles loading one 3d model, ideally it would load more than that,
* 	and it would do it in a multi-threaded fashion.
*
*	- [@TODO] This system should be able to load and unload different 3D models.
*	- [@TODO]This system also loads fonts.
*
******************************************************************************/

#pragma once

#include "mesh/mesh.h"
#include "parsers/parser.h"
#include "parsers/wavefrontobj/wavefrontobj.h"

namespace Starlight {
	namespace ResourceManager {
		internal MeshInfo* load_model(Arena* arena, string8 file);
		internal void load_font();
	}
}

/*
Some notes about this

Currently this only caters about loading one 3d model at application startup. Ideally this would be done asynchronously in a background thread, i.e. Asset Streaming., with the ability to load different types of models, animation, etc.

Another limitiation is that it loads one file for the application lifetime. Ideally, it would 

*/
//internal void resource_manager_init(void);
