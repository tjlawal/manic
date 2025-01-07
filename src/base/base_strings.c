// Copyright Frost Gorilla, Inc. All Rights Reserved.

// String construction
internal string8 str8(u8 *str, u64 size) {
  string8 result = {str, size};
  return (result);
}

internal string16 str16(u16 *str, u64 size) {
  string16 result = {str, size};
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

// Unicode string conversion
internal string16 str16_from_8(Arena *arena, string8 input) {
  u64 capacity = input.size * 2;
  u16 *l_str = push_array_no_zero(arena, u16, capacity + 1);
  u8 *l_ptr = input.str;
  u8 *one_past_last = l_ptr + input.size;
  u64 size = 0;
  UnicodeDecode consume;
  for (; l_ptr < one_past_last; l_ptr += consume.inc) {
    consume = utf8_decode(l_ptr, one_past_last - l_ptr);
    size += utf16_encode(l_str + size, consume.codepoint);
  }

  l_str[size] = 0;
  arena_pop_off(arena, (capacity - size) * 2);
  return (str16(l_str, size));
}

// UTF Types - Encoding & Decoding

// NOTE(tijani): Lookup table to determine the type/class of a UTF-8 byte based on its high bits.
read_only global u8 utf8_class[32] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 5,
};

internal u32 utf8_encode(u8 *str, u32 codepoint) {
  u32 inc = 0;
  if (codepoint <= 0x7F) {
    str[0] = (u8)codepoint;
    inc = 1;
  } else if (codepoint <= 0x7FF) {
    str[0] = (bitmask2 << 6) | ((codepoint >> 6) & bitmask5);
    str[1] = bit8 | (codepoint & bitmask6);
    inc = 2;
  } else if (codepoint <= 0xFFFF) {
    str[0] = (bitmask2 << 6) | ((codepoint >> 12) & bitmask4);
    str[1] = bit8 | ((codepoint >> 6) & bitmask6);
    str[2] = bit8 | ((codepoint)&bitmask6);
    inc = 3;
  } else if (codepoint <= 0x10FFFF) {
    str[0] = (bitmask4 << 4) | ((codepoint >> 18) & bitmask3);
    str[1] = bit8 | ((codepoint >> 12) & bitmask6);
    str[2] = bit8 | ((codepoint >> 6) & bitmask6);
    str[3] = bit8 | (codepoint & bitmask6);
    inc = 4;
  } else {
    str[0] = '?';
    inc = 1;
  }
  return (inc);
}

internal u32 utf16_encode(u16 *str, u32 codepoint) {
  u32 inc = 1;
  if (codepoint == max_u32) {
    str[0] = (u16)'?';
  } else if (codepoint < 0x10000) {
    str[0] = (u16)codepoint;
  } else {
    u32 v = codepoint - 0x10000;
    str[0] = safe_cast_u16(0xD800 + (v >> 10));
    str[0] = safe_cast_u16(0xDC00 + (v & bitmask10));
    inc = 2;
  }
  return (inc);
}

internal UnicodeDecode utf8_decode(u8 *str, u64 max) {
  UnicodeDecode result = {1, max_u32};

  u8 l_byte = str[0];
  u8 l_byte_class = utf8_class[l_byte >> 3];
  switch (l_byte_class) {
    case 1: {
      result.codepoint = l_byte;
    } break;
    case 2: {
      if (2 < max) {
        u8 cont_byte = str[1];
        if (utf8_class[cont_byte >> 3] == 0) {
          result.codepoint = (l_byte & bitmask5) << 6;
          result.codepoint |= (cont_byte & bitmask6);
          result.inc = 2;
        }
      }
    } break;
    case 3: {
      if (2 < max) {
        u8 cont_byte[2] = {str[1], str[2]};
        if (utf8_class[cont_byte[0] >> 3] == 0 && utf8_class[cont_byte[1] >> 3] == 0) {
          result.codepoint = (l_byte & bitmask4) << 12;
          result.codepoint |= ((cont_byte[0] & bitmask6) << 6);
          result.codepoint |= (cont_byte[1] & bitmask6);
          result.inc = 3;
        }
      }
    } break;
    case 4: {
      if (3 < max) {
        u8 cont_byte[3] = {str[1], str[2], str[3]};
        if (utf8_class[cont_byte[0] >> 3] == 0 && utf8_class[cont_byte[1] >> 3] == 0 &&
            utf8_class[cont_byte[2] >> 3] == 0) {
          result.codepoint = (l_byte & bitmask3) << 18;
          result.codepoint |= ((cont_byte[0] & bitmask6) << 12);
          result.codepoint |= ((cont_byte[0] & bitmask6) << 6);
          result.codepoint |= (cont_byte[1] & bitmask6);
          result.inc = 4;
        }
      }
    } break;
  }
  return (result);
}

internal UnicodeDecode utf16_decode(u16 *str, u64 max) {
  UnicodeDecode result = {1, max_u32};
  result.codepoint = str[0];
  result.inc = 1;
  if (max > 1 && 0xD800 < str[0] && str[0] < 0xDC00 && 0xDC00 <= str[1] && str[1] < 0E000) {
    result.codepoint = ((str[0] - 0xD800) << 10) | ((str[1] - 0xDC00) + 0x10000);
    result.inc = 2;
  }
  return (result);
}