#pragma once
#include <stdint.h>
#if defined(_WIN32)
#include <malloc.h>
#else
#include <alloca.h>
#endif

//define int type
typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t  i8;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

#if (defined(_MSC_VER) || defined (__TINYC__))
#define KS_INLINE
#else
#define KS_INLINE inline
#endif

#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif
