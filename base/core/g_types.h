#ifndef GLINT_BASE_CORE_TYPES_H
#define GLINT_BASE_CORE_TYPES_H

#include <stdint.h>

#define internal static
#define global static
#define local_persist static
typedef uint8_t	 u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t	 i8;
typedef int16_t	 i16;
typedef int32_t	 i32;
typedef int64_t	 i64;
typedef i8		 b8;
typedef i16		 b16;
typedef i32		 b32;
typedef i64		 b64;
typedef float	 f32;
typedef double	 f64;

typedef u64 version;

typedef union u128 u128;
union u128
{
	u8	u8[16];
	u16 u16[8];
	u32 u32[4];
	u64 u64[2];
	f32 f32[4];
	f64 f64[2];
};

typedef union u256 u256;
union u256
{
	u8	 u8[32];
	u16	 u16[16];
	u32	 u32[8];
	u64	 u64[4];
	u128 u128[2];
	f32	 f32[8];
	f64	 f64[4];
};

typedef union u512 u512;
union u512
{
	u8	 u8[64];
	u16	 u16[32];
	u32	 u32[16];
	u64	 u64[8];
	u128 u128[4];
	u256 u256[2];
	f32	 f32[16];
	f64	 f64[8];
};

typedef struct range_i32
{
	i32 min;
	i32 max;
} range_i32;

typedef struct range_u32
{
	u32 min;
	u32 max;
} range_u32;

typedef struct range_i64
{
	i64 min;
	i64 max;
} range_i64;

typedef struct range_u64
{
	u64 min;
	u64 max;
} range_u64;

#endif // GLINT_BASE_CORE_TYPES_H
