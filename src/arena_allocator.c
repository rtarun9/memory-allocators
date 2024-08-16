#include "common.h"

#include <stdio.h>

// The arena is a linear allocator, which has a base pointer and offset.
// When alloc is called, the pointer returned is the base_ptr + offset. Then,
// the offset is adjusted accordingly.
typedef struct {
  u8 *base_ptr;
  size_t offset;
  size_t size;
} arena_t;

void arena_init(arena_t *arena, size_t arena_size) {
  ASSERT(arena);

  arena->base_ptr = (u8 *)malloc(arena_size);
  arena->offset = 0;
  arena->size = arena_size;
}

void arena_free(arena_t *arena) {
  ASSERT(arena);
  ASSERT(arena->base_ptr);

  free(arena->base_ptr);
}

#define DEFAULT_ALIGNMENT_VALUE sizeof(i32)

// NOTE: Alignment and size are different, as the same function can be used to
// allocate arrays. In such cases, size will be size of the array, while
// alignment will be the alignment required by each element of the array.
u8 *arena_alloc_array(arena_t *arena, size_t num_elements,
                      size_t size_per_element, size_t alignment) {
  ASSERT(is_power_of_2(alignment));

  ASSERT(arena);
  ASSERT(arena->base_ptr);

  size_t size = num_elements * size_per_element;

  // Corrected result with the pointer returned takes into account memory
  // alignment.
  // NOTE: This computation should note take size into account, as size will
  // only determine the alignemnt and new offset for future arena allocations.
  u8 *memory_aligned_result =
      align_memory_address(arena->base_ptr + arena->offset, alignment);

  // Get the relative offset from the base_ptr.
  size_t base_ptr_relative_offset = memory_aligned_result - arena->base_ptr;

  if (base_ptr_relative_offset + size > arena->size) {
    return NULL;
  }

  arena->offset = base_ptr_relative_offset + size;

  ASSERT((size_t)(memory_aligned_result) % alignment == 0);

  return memory_aligned_result;
}

u8 *arena_alloc_aligned(arena_t *arena, size_t size, size_t alignment) {
  return arena_alloc_array(arena, 1, size, alignment);
}

u8 *arena_alloc_default_aligned(arena_t *arena, size_t size) {
  return arena_alloc_array(arena, 1, size, DEFAULT_ALIGNMENT_VALUE);
}

void arena_reset(arena_t *arena) {
  ASSERT(arena);
  ASSERT(arena->base_ptr);

  arena->offset = 0;
}

int main() {
  // Checks to see if align_memory_address works as expected.
  u8 bytes[512];

  // Size's (that must be multiples of 2)
  // Check for 1 byte offset.
  u8 *one_byte_offset = align_memory_address(bytes, 1);
  u8 *one_byte_offset_2 = align_memory_address(bytes + 1, 1);
  ASSERT((size_t)one_byte_offset % 1 == 0);
  ASSERT((size_t)one_byte_offset_2 % 1 == 0);

  // Check for 2 byte offset.
  u8 *two_byte_offset = align_memory_address(bytes + 1, 2);
  u8 *two_byte_offset_2 = align_memory_address(bytes + 3, 2);
  u8 *two_byte_offset_3 = align_memory_address(bytes + 4, 2);
  ASSERT((size_t)two_byte_offset % 2 == 0);
  ASSERT((size_t)two_byte_offset_2 % 2 == 0);
  ASSERT((size_t)two_byte_offset_3 % 2 == 0);

  // Check for 4 byte offset.
  u8 *four_byte_offset = align_memory_address(bytes + 1, 4);
  u8 *four_byte_offset_2 = align_memory_address(bytes + 3, 4);
  u8 *four_byte_offset_3 = align_memory_address(bytes + 4, 4);
  ASSERT((size_t)four_byte_offset % 4 == 0);
  ASSERT((size_t)four_byte_offset_2 % 4 == 0);
  ASSERT((size_t)four_byte_offset_3 % 4 == 0);

  // Check for 8 byte offset.
  u8 *eight_byte_offset = align_memory_address(bytes + 0, 8);
  u8 *eight_byte_offset_2 = align_memory_address(bytes + 3, 8);
  u8 *eight_byte_offset_3 = align_memory_address(bytes + 5, 4);
  u8 *eight_byte_offset_4 = align_memory_address(bytes + 6, 4);
  ASSERT((size_t)eight_byte_offset % 8 == 0);
  ASSERT((size_t)eight_byte_offset_2 % 8 == 0);
  ASSERT((size_t)eight_byte_offset_3 % 8 == 0);
  ASSERT((size_t)eight_byte_offset_4 % 8 == 0);

  // Test the actual arena allocator.
  arena_t arena;
  arena_init(&arena, KILOBYTE(512));

  u8 *allocated_array = arena_alloc_array(&arena, 7, 3, 4);

  u8 *default_aligned_base = arena_alloc_default_aligned(&arena, KILOBYTE(1));
  u8 *default_aligned_base_2 =
      arena_alloc_default_aligned(&arena, KILOBYTE(15));
  u8 *default_aligned_base_3 =
      arena_alloc_default_aligned(&arena, KILOBYTE(32));
  u8 *default_aligned_base_4 =
      arena_alloc_default_aligned(&arena, KILOBYTE(56));

  arena_free(&arena);

  printf("Code ran successfully!");

  return 0;
}
