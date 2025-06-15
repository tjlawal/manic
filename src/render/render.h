
#pragma once

namespace Starlight {
	namespace Render {

		union RenderHandle {
			u64 handle[1];
		};

	}
}


#include "render/core/render_core.h"

#if OS_WINDOWS
	#include "render/software/win32/render_win32.h"
#else
	#error "This rendering backend is not supported yet!"
#endif 

