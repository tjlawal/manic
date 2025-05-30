
namespace Starlight {
	namespace Platform {

		internal Handle handle_zero(void) { return Handle{0}; }

		internal b32 handle_match(Handle a, Handle b) { return a.handle[0] == b.handle[0]; }
	}
}