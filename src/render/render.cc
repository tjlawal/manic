#include "render/core/render_core.cc"

#if OS_WINDOWS
	#include "render/software/win32/render_win32.cc"
#else
	#error "This rendering backend is not supported yet!"
#endif 

