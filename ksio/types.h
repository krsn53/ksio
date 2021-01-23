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

#if (defined(_MSC_VER))
#define KS_INLINE __inline
#elif (defined(__TINYCC__))
#define KS_INLINE
#else
#define KS_INLINE __inline__
#endif

#if (defined(_MSC_VER))
#define KS_FORCEINLINE __forceinline
#elif (defined(__TINYCC__))
#define KS_FORCEINLINE
#else
#define KS_FORCEINLINE __attribute__((always_inline))
#endif

#if (defined(_MSC_VER) || defined (__TINYC__))
#define KS_NOINLINE
#else
#define KS_NOINLINE __attribute__((noinline))
#endif

#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif
