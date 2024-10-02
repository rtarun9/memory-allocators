#include "common.h"

#include <stdio.h>

// The stack allocator is a evolution of the arena allocator.
// The main difference is that the allocator 'offset' can move backward, if a particular allocation is freed.
typedef struct
{
    u8 *base_ptr;
    size_t offset;
    size_t size;
} stack_allocator_t;

void stack_allocator_init(stack_allocator_t *allocator, size_t size)
{
    ASSERT(allocator);

    allocator->base_ptr = (u8 *)malloc(size);
    allocator->offset = 0;
    allocator->size = size;
}

#define DEFAULT_ALIGNMENT sizeof(int)
u8 *stack_allocator_push(stack_allocator_t *allocator, size_t size, size_t alignment)
{
    ASSERT(allocator);
    ASSERT(allocator->base_ptr);
    ASSERT(is_power_of_2(alignment));

    // First, get the  memory aligned pointer to start of new allocated memory.
    u8 *memory_aligned_base_ptr = align_memory_address(allocator->base_ptr + allocator->offset, alignment);
    size_t memory_aligned_offset = memory_aligned_base_ptr - allocator->base_ptr;

    // Check if the allocation if possible.
    if (memory_aligned_offset + size > allocator->size)
    {
        return NULL;
    }

    allocator->offset = memory_aligned_offset + size;

    return memory_aligned_base_ptr;
}

void stack_allocator_pop(stack_allocator_t *allocator, size_t size)
{
    ASSERT(allocator);
    ASSERT(allocator->base_ptr);
    ASSERT(size);

    if ((i64)(allocator->size - size) < 0)
    {
        return;
    }

    allocator->offset -= size;
}

int main()
{
    stack_allocator_t allocator = {0};
    stack_allocator_init(&allocator, KILOBYTE(4));

    u8 *array_a = stack_allocator_push(&allocator, sizeof(int) * 8, DEFAULT_ALIGNMENT);
    u8 *array_b = stack_allocator_push(&allocator, sizeof(float) * 16, DEFAULT_ALIGNMENT);

    stack_allocator_pop(&allocator, sizeof(float) * 16);

    printf("Code has run succesfully!");

    return 0;
}
