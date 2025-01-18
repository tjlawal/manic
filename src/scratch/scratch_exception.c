// Copyright Frost Gorilla, Inc. All Rights Reserved.

#define BUILD_CONSOLE_INTERFACE 1

// clang-format off
#include "base/base_inc.h"
#include "os/os_inc.h"


#include "base/base_inc.c"
#include "os/os_inc.c"
// clang-format on

#include <shlwapi.h>

void entry_point() {
  // Test exception on the terminal
  // int *ptr = NULL;
  //*ptr = 5;

  // printf("%d", ptr);

  int z = 0;
  int r = 1 / z;

  printf("%d", r);
}