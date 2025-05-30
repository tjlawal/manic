#ifndef RENDER_INC_H
#define RENDER_INC_H

#include "render/core/render_core.h"

#if OS_WINDOWS
	#include "render/software/win32/render_win32.h"
#else
	#error "This rendering backend is not supported yet!"
#endif 


#endif // RENDER_INC_H