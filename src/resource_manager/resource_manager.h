#pragma once

#include "mesh.h"

// File Format Parsers
#include "parsers/parser.h"
#include "parsers/wavefrontobj.h"

/* Resource Manager 

	Provide an ease to use API that works across multiples 3D file format.

	*/
namespace Starlight {
	namespace ResourceManager {
		
			
		// Initialization
		internal void resource_manager_init(void);

	}

}