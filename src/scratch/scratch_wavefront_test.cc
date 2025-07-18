#define BUILD_TITLE                        "Wavefront OBJ Parser Test"
#define BUILD_VERSION_MAJOR                00
#define BUILD_VERSION_MINOR                00
#define BUILD_VERSION_PATCH                01
#define BUILD_RELEASE_PHASE_STRING_LITERAL "Alpha"
#define BUILD_CONSOLE_INTERFACE						 1

// Includes, order is important here.
#include "foundation/context_switch.h"
#include "third_party/third_party.h"

// [.h]
#include "foundation/foundation.h"
#include "platform/platform.h"
#include "resource_manager/resource_manager.h"

// [.c]
#include "foundation/foundation.cc"
#include "platform/platform.cc"
#include "resource_manager/resource_manager.cc"

using namespace Starlight::Foundation;
using namespace Starlight::Platform;
using namespace Starlight::ResourceManager;


namespace Starlight {

	internal void update() {}

	internal void render() {}

	internal void main_loop() {
		printf("Hello, World");
	}
}