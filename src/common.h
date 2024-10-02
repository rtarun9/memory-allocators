#ifndef __COMMON__H
#define __COMMON__H

// Aliases for data types.
#include <stdint.h>
#include <stdlib.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

// Aliases for the static keyword.
#define internal static
#define global static
#define local_persist static

// #defines that are storage related.
#define BYTE(x) (x)
#define KILOBYTE(x) (x * 1024LL)
#define MEGABYTE(x) (KILOBYTE(x) * 1024LL)
#define GIGABYTE(x) (MEGABYTE(x) * 1024LL)

// Assert / Debug related.
#ifdef DEBUG_MODE
#define ASSERT(x)                                                                                                      \
    if (!(x))                                                                                                          \
    {                                                                                                                  \
        u8 *ptr = NULL;                                                                                                \
        *ptr = 0;                                                                                                      \
    }
#endif

#ifndef DEBUG_MODE
#define ASSERT(x)
#endif

// Commonly used math functions.
internal i32 is_power_of_2(size_t value)
{ // In binary, if a number if a power of 2,
  // then only a SINGLE bit will be set.
  // Moreoever, value - 1 will be all 1's.

    return (value != 0) && (value & (value - 1)) == 0;
}

internal u8 *align_memory_address(u8 *value, size_t alignment_size)
{
    ASSERT(is_power_of_2(alignment_size) == 1);

    // As alignment size will be a power of 2, to do a fast modulus operator doing
    // value & (alignment_size - 1) will be sufficient.
    size_t modulo = (size_t)value & (alignment_size - 1);
    size_t result = (size_t)value;

    if (modulo != 0)
    {
        result += alignment_size - modulo;
    }

    return (u8 *)result;
}

#endif
