// Copyright Frost Gorilla, Inc. All Rights Reserved.

#define BUILD_CONSOLE_INTERFACE 1

// clang-format off
#include "base/base_inc.h"
#include "os/os_inc.h"


#include "base/base_inc.c"
#include "os/os_inc.c"
// clang-format on

void entry_point() {

  // ArenaParams params = {.reserve_size = MB(40), .commit_size = KB(64), .flags
  // = 0, .optional_preallocated_buffer = 0};
  Arena *temp_memory = arena_alloc();

  typedef struct User {
    string8 name;
    int age;
  } User;

  Temp scratch = temp_begin(temp_memory);

  // Allocate array of users
  User *users = push_array(scratch.arena, User, 3);

  // Initialize users
  users[0].name = push_str8_copy(scratch.arena, str8_lit("Alice"));
  users[0].age = 25;

  users[1].name = push_str8_copy(scratch.arena, str8_lit("Bob"));
  users[1].age = 30;

  users[2].name = push_str8_copy(scratch.arena, str8_lit("Charlie"));
  users[2].age = 35;

  // Print all users
  for (int i = 0; i < 3; i++) {
    printf("User: %.*s, Age: %d\n", (int)users[i].name.size, users[i].name.str, users[i].age);
  }

  temp_end(scratch); // Frees all allocated memory
  arena_release(temp_memory);
}