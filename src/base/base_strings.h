// Copyright Frost Gorilla, Inc. All Rights Reserved.

#ifndef BASE_STRINGS_H
#define BASE_STRINGS_H

// String types
typedef struct string8 string8;
struct string8 {
  u8 *str;
  u64 size;
};

typedef struct string16 string16;
struct string16 {
  u16 *str;
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
internal string8 str8(u8 *str, u64 size);
internal string16 str16(u16 *str, u64 size);
#define str8_lit(string)      str8((u8 *)(string), sizeof(string) - 1)
#define str8_lit_comp(string) {(u8 *)(string), sizeof(string) - 1}

// String copying
internal string8 push_str8_copy(Arena *arena, string8 string);

// Unicode string conversion
internal string16 str16_from_8(Arena *arena, string8 input);

// UTF Types - Encoding & Decoding
typedef struct UnicodeDecode UnicodeDecode;
struct UnicodeDecode {
  u32 inc;
  u32 codepoint;
};

internal u32 utf8_encode(u8 *str, u32 codepoint);
internal u32 utf16_encode(u16 *str, u32 codepoint);
internal UnicodeDecode utf8_decode(u8 *str, u64 max);
internal UnicodeDecode utf18_decode(u16 *str, u64 max);

#endif // BASE_STRINGS_H