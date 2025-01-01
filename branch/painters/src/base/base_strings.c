// Copyright Frost Gorilla, Inc. All Rights Reserved.

// String construction
internal string8 str8(u8 *str, u64 size) {
  string8 result = {str, size};
  return (result);
}

// String formatting
internal string8 push_str8fv(Arena *arena, char *format, va_list args) {
  va_list l_args;
  va_copy(l_args, args);
  u32 needed_bytes = vsnprintf(0, 0, format, args) + 1;
  string8 result = {0};
  result.str = push_array_no_zero(arena, u8, needed_bytes);
  result.size = vsnprintf((char *)result.str, needed_bytes, format, l_args);
  result.str[result.size] = 0;
  va_end(l_args);
  return (result);
}

// String copying
internal string8 push_str8_copy(Arena *arena, string8 string) {
  string8 l_string;
  l_string.size = string.size;
  l_string.str = push_array_no_zero(arena, u8, l_string.size + 1);
  memory_copy(l_string.str, string.str, string.size);
  l_string.str[l_string.size] = 0; // NOTE(tijani): Set null terminator
  return (l_string);
}