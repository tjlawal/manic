// Copyright Frost Gorilla, Inc. All Rights Reserved.

#ifndef BASE_STRINGS_H
#define BASE_STRINGS_H

// String types
typedef struct string8 string8;
struct string8 {
  u8 *str;
  u64 size;
};

// String list
typedef struct string8node string8node;
struct string8node {
  string8node *next;
  string8 string;
};

typedef struct string8list string8list;
struct string8list {
  string8node *first;
  string8node *last;
  u64 node_count;
  u64 total_size;
};

// String construction
#define str8_lit(string)      str8((u8 *)(string), sizeof(string) - 1)
#define str8_lit_comp(string) {(u8 *)(string), sizeof(string) - 1}

internal string8 str8(u8 *str, u64 size);

// String copying
internal string8 push_str8_copy(Arena *arena, string8 string);

#endif // BASE_STRINGS_H