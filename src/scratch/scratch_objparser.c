// Copyright Frost Gorilla, Inc. All Rights Reserved.

#define BUILD_CONSOLE_INTERFACE 1

// clang-format off
#include "base/base_inc.h"
#include "os/os_inc.h"


#include "base/base_inc.c"
#include "os/os_inc.c"
// clang-format on

void entry_point() {
  Temp scratch = scratch_begin(0, 0);

  OS_Handle file = os_file_open(OS_AccessFlag_Read, "data/meshes/f22.obj");

  os_file_read(file, , );

  scratch_end(scratch);
}