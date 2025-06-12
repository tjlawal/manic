
using namespace Starlight::Platform;

namespace Starlight {

	internal void main_thread_entry_point(){
		ProfBlock(0, profDebug_darkmagenta);
		Temp scratch = scratch_begin(0, 0);
		//ThreadNameF("[main thread]");

		Gfx::gfx_init(); 

		main_loop();

		scratch_end(scratch);
	}
}