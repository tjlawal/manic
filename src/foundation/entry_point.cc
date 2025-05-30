
using namespace Starlight::Platform;

namespace Starlight {

	internal void main_thread_entry_point(){
		Temp scratch = scratch_begin(0, 0);
		//ThreadNameF("[main thread]");

		Gfx::gfx_init(); 

		Platform::MainLoop();

		scratch_end(scratch);
	}
}