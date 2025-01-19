// Copyright Frost Gorilla, Inc. All Rights Reserved.

// Safe casts
internal u16 safe_cast_u16(u32 x) {
  assert_always(x <= max_u16);
  u16 result = (u16)x;
  return result;
}

internal u32 u32_from_u64_saturate(u64 x) {
  u32 x32 = (x > max_u32) ? max_u32 : (u32)x;
  return x32;
}

/// TODO(tijani): CRUFT, delete and use arenas in its place of something else.

#define ARRAY_RAW_DATA(array) ((int *)(array) - 2)
#define ARRAY_CAPACITY(array) (ARRAY_RAW_DATA(array)[0])
#define ARRAY_OCCUPIED(array) (ARRAY_RAW_DATA(array)[1])

void *array_hold(void *array, int count, int item_size) {
  if (array == NULL) {
    int raw_size = (sizeof(int) * 2) + (item_size * count);
    int *base = (int *)malloc(raw_size);
    base[0] = count; // capacity
    base[1] = count; // occupied
    return base + 2;
  } else if (ARRAY_OCCUPIED(array) + count < ARRAY_CAPACITY(array)) {
    ARRAY_OCCUPIED(array) += count;
    return array;
  } else {
    int needed_size = ARRAY_OCCUPIED(array) + count;
    int double_curr = ARRAY_CAPACITY(array) * 2;
    int capacity = needed_size > double_curr ? needed_size : double_curr;
    int occupied = needed_size;
    int raw_size = sizeof(int) * 2 + item_size * capacity;
    int *base = (int *)realloc(ARRAY_RAW_DATA(array), raw_size);
    base[0] = capacity;
    base[1] = occupied;
    return base + 2;
  }
}

int array_length(void *array) { return (array != NULL) ? ARRAY_OCCUPIED(array) : 0; }

void array_free(void *array) {
  if (array != NULL) {
    free(ARRAY_RAW_DATA(array));
  }
}
