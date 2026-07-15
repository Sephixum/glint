#ifndef GLINT_BASE_UTIL_UTIL_H
#define GLINT_BASE_UTIL_UTIL_H

#include "../core/g_platform_and_compiler_defines.h"
#include "../core/g_types.h"

#define g_min(A, B) (((A) < (B)) ? (A) : (B))
#define g_max(A, B) (((A) > (B)) ? (A) : (B))
#define g_clamp_top(A, X) g_min(A, X)
#define g_clamp_bot(X, B) g_max(X, B)
#define g_clamp(A, X, B) (((X) < (A)) ? (A) : ((X) > (B)) ? (B) : (X))

#define g_member(T, m) (((T*)0)->m)
#define g_offset_of(T, m) offsetof(T, m)
#define g_member_from_offset(T, ptr, off) (T)((((u8*)ptr) + (off)))
#define g_member_from_ptr(T, ptr, m) (void*)((((u8*)ptr) + g_offset_of(T, m)))
#define g_cast_from_member(T, m, ptr) (T*)(((u8*)ptr) - g_offset_of(T, m))

#define g_compose_64bit(a, b) ((((u64)a) << 32) | ((u64)b))
#define g_compose_32bit(a, b) ((((u32)a) << 16) | ((u32)b))
#define g_align_pow2(x, b) (((x) + (b) - 1) & (~((b) - 1)))
#define g_align_down_pow2(x, b) ((x) & (~((b) - 1)))
#define g_align_pad_pow2(x, b) ((0 - (x)) & ((b) - 1))
#define g_is_pow2(x) ((x) != 0 && ((x) & ((x) - 1)) == 0)
#define g_is_pow2_or_zero(x) ((((x) - 1) & (x)) == 0)

#define g_version(major, minor, patch)                                                                                 \
	(u64)((((u64)(major) & 0xffff) << 32) | ((((u64)(minor) & 0xffff) << 16)) | ((((u64)(patch) & 0xffff) << 0)))
#define g_major_from_version(version) (((version) & 0xffff00000000ull) >> 32)
#define g_minor_from_version(version) (((version) & 0x0000ffff0000ull) >> 16)
#define g_patch_from_version(version) (((version) & 0x00000000ffffull) >> 0)

#define g_defer_scope(begin, end) for (int _i_ = ((begin), 0); !_i_; _i_ += 1, (end))
#define g_defer_scope_checked(begin, end) for (int _i_ = 2 * !(begin); (_i_ == 2 ? ((end), 0) : !_i_); _i_ += 1, (end))

#define g_for_each_index(it, count) for (u64 it = 0; it < (count); it += 1)
#define g_for_each_element(it, array) for (u64 it = 0; it < g_array_count(array); it += 1)
#define g_for_each_enum_value(type, it) for (type it = (type)0; it < type##_COUNT; it = (type)(it + 1))
#define g_for_each_nonzero_enum_value(type, it) for (type it = (type)1; it < type##_COUNT; it = (type)(it + 1))
#define g_for_each_in_range(it, range) for (i64 it = (range).min; it < (range).max; it += 1)
#define g_for_each_node(it, T, first) for (T* it = first; it != 0; it = it->next)
#define g_for_each_bit(it, flags)                                                                                      \
	(u64(_i_) = (flags), it = (flags) & -(flags); (_i_) != 0; (_i_) &= ((_i_) - 1), it = (flags) & -(flags))

#define g_kb(n) (((u64)(n)) << 10)
#define g_mb(n) (((u64)(n)) << 20)
#define g_gb(n) (((u64)(n)) << 30)
#define g_tb(n) (((u64)(n)) << 40)
#define g_thousand(n) ((n) * 1000)
#define g_million(n) ((n) * 1000000)
#define g_billion(n) ((n) * 1000000000)

#define g_stringify_(S) #S
#define g_stringify(S) g_stringify_(S)

#define g_glue_(A, B) A##B
#define g_glue(A, B) g_glue_(A, B)

#define g_array_count(a) (sizeof(a) / sizeof((a)[0]))

#define g_swap(T, a, b)                                                                                                \
	do                                                                                                                 \
	{                                                                                                                  \
		T t__ = a;                                                                                                     \
		a	  = b;                                                                                                     \
		b	  = t__;                                                                                                   \
	} while (0)

global constexpr u32	  bitmask1	= 0x00000001;
global constexpr u32	  bitmask2	= 0x00000003;
global constexpr u32	  bitmask3	= 0x00000007;
global constexpr u32	  bitmask4	= 0x0000000f;
global constexpr u32	  bitmask5	= 0x0000001f;
global constexpr u32	  bitmask6	= 0x0000003f;
global constexpr u32	  bitmask7	= 0x0000007f;
global constexpr u32	  bitmask8	= 0x000000ff;
global constexpr u32	  bitmask9	= 0x000001ff;
global constexpr u32	  bitmask10 = 0x000003ff;
global constexpr u32	  bitmask11 = 0x000007ff;
global constexpr u32	  bitmask12 = 0x00000fff;
global constexpr u32	  bitmask13 = 0x00001fff;
global constexpr u32	  bitmask14 = 0x00003fff;
global constexpr u32	  bitmask15 = 0x00007fff;
global constexpr u32	  bitmask16 = 0x0000ffff;
global constexpr u32	  bitmask17 = 0x0001ffff;
global constexpr u32	  bitmask18 = 0x0003ffff;
global constexpr u32	  bitmask19 = 0x0007ffff;
global constexpr u32	  bitmask20 = 0x000fffff;
global constexpr u32	  bitmask21 = 0x001fffff;
global constexpr u32	  bitmask22 = 0x003fffff;
global constexpr u32	  bitmask23 = 0x007fffff;
global constexpr u32	  bitmask24 = 0x00ffffff;
global constexpr u32	  bitmask25 = 0x01ffffff;
global constexpr u32	  bitmask26 = 0x03ffffff;
global constexpr u32	  bitmask27 = 0x07ffffff;
global constexpr u32	  bitmask28 = 0x0fffffff;
global constexpr u32	  bitmask29 = 0x1fffffff;
global constexpr u32	  bitmask30 = 0x3fffffff;
global constexpr u32	  bitmask31 = 0x7fffffff;
global constexpr u32	  bitmask32 = 0xffffffff;
expr global constexpr u64 bitmask33 = 0x00000001ffffffffull;
global constexpr u64	  bitmask34 = 0x00000003ffffffffull;
global constexpr u64	  bitmask35 = 0x00000007ffffffffull;
global constexpr u64	  bitmask36 = 0x0000000fffffffffull;
global constexpr u64	  bitmask37 = 0x0000001fffffffffull;
global constexpr u64	  bitmask38 = 0x0000003fffffffffull;
global constexpr u64	  bitmask39 = 0x0000007fffffffffull;
global constexpr u64	  bitmask40 = 0x000000ffffffffffull;
global constexpr u64	  bitmask41 = 0x000001ffffffffffull;
global constexpr u64	  bitmask42 = 0x000003ffffffffffull;
global constexpr u64	  bitmask43 = 0x000007ffffffffffull;
global constexpr u64	  bitmask44 = 0x00000fffffffffffull;
global constexpr u64	  bitmask45 = 0x00001fffffffffffull;
global constexpr u64	  bitmask46 = 0x00003fffffffffffull;
global constexpr u64	  bitmask47 = 0x00007fffffffffffull;
global constexpr u64	  bitmask48 = 0x0000ffffffffffffull;
global constexpr u64	  bitmask49 = 0x0001ffffffffffffull;
global constexpr u64	  bitmask50 = 0x0003ffffffffffffull;
global constexpr u64	  bitmask51 = 0x0007ffffffffffffull;
global constexpr u64	  bitmask52 = 0x000fffffffffffffull;
global constexpr u64	  bitmask53 = 0x001fffffffffffffull;
global constexpr u64	  bitmask54 = 0x003fffffffffffffull;
global constexpr u64	  bitmask55 = 0x007fffffffffffffull;
global constexpr u64	  bitmask56 = 0x00ffffffffffffffull;
global constexpr u64	  bitmask57 = 0x01ffffffffffffffull;
global constexpr u64	  bitmask58 = 0x03ffffffffffffffull;
global constexpr u64	  bitmask59 = 0x07ffffffffffffffull;
global constexpr u64	  bitmask60 = 0x0fffffffffffffffull;
global constexpr u64	  bitmask61 = 0x1fffffffffffffffull;
global constexpr u64	  bitmask62 = 0x3fffffffffffffffull;
global constexpr u64	  bitmask63 = 0x7fffffffffffffffull;
global constexpr u64	  bitmask64 = 0xffffffffffffffffull;
expr global constexpr u32 bit1		= (1 << 0);
global constexpr u32	  bit2		= (1 << 1);
global constexpr u32	  bit3		= (1 << 2);
global constexpr u32	  bit4		= (1 << 3);
global constexpr u32	  bit5		= (1 << 4);
global constexpr u32	  bit6		= (1 << 5);
global constexpr u32	  bit7		= (1 << 6);
global constexpr u32	  bit8		= (1 << 7);
global constexpr u32	  bit9		= (1 << 8);
global constexpr u32	  bit10		= (1 << 9);
global constexpr u32	  bit11		= (1 << 10);
global constexpr u32	  bit12		= (1 << 11);
global constexpr u32	  bit13		= (1 << 12);
global constexpr u32	  bit14		= (1 << 13);
global constexpr u32	  bit15		= (1 << 14);
global constexpr u32	  bit16		= (1 << 15);
global constexpr u32	  bit17		= (1 << 16);
global constexpr u32	  bit18		= (1 << 17);
global constexpr u32	  bit19		= (1 << 18);
global constexpr u32	  bit20		= (1 << 19);
global constexpr u32	  bit21		= (1 << 20);
global constexpr u32	  bit22		= (1 << 21);
global constexpr u32	  bit23		= (1 << 22);
global constexpr u32	  bit24		= (1 << 23);
global constexpr u32	  bit25		= (1 << 24);
global constexpr u32	  bit26		= (1 << 25);
global constexpr u32	  bit27		= (1 << 26);
global constexpr u32	  bit28		= (1 << 27);
global constexpr u32	  bit29		= (1 << 28);
global constexpr u32	  bit30		= (1 << 29);
global constexpr u32	  bit31		= (1 << 30);
global constexpr u32	  bit32		= (1 << 31);
expr global constexpr u64 bit33		= (1ull << 32);
global constexpr u64	  bit34		= (1ull << 33);
global constexpr u64	  bit35		= (1ull << 34);
global constexpr u64	  bit36		= (1ull << 35);
global constexpr u64	  bit37		= (1ull << 36);
global constexpr u64	  bit38		= (1ull << 37);
global constexpr u64	  bit39		= (1ull << 38);
global constexpr u64	  bit40		= (1ull << 39);
global constexpr u64	  bit41		= (1ull << 40);
global constexpr u64	  bit42		= (1ull << 41);
global constexpr u64	  bit43		= (1ull << 42);
global constexpr u64	  bit44		= (1ull << 43);
global constexpr u64	  bit45		= (1ull << 44);
global constexpr u64	  bit46		= (1ull << 45);
global constexpr u64	  bit47		= (1ull << 46);
global constexpr u64	  bit48		= (1ull << 47);
global constexpr u64	  bit49		= (1ull << 48);
global constexpr u64	  bit50		= (1ull << 49);
global constexpr u64	  bit51		= (1ull << 50);
global constexpr u64	  bit52		= (1ull << 51);
global constexpr u64	  bit53		= (1ull << 52);
global constexpr u64	  bit54		= (1ull << 53);
global constexpr u64	  bit55		= (1ull << 54);
global constexpr u64	  bit56		= (1ull << 55);
global constexpr u64	  bit57		= (1ull << 56);
global constexpr u64	  bit58		= (1ull << 57);
global constexpr u64	  bit59		= (1ull << 58);
global constexpr u64	  bit60		= (1ull << 59);
global constexpr u64	  bit61		= (1ull << 60);
global constexpr u64	  bit62		= (1ull << 61);
global constexpr u64	  bit63		= (1ull << 62);
global constexpr u64	  bit64		= (1ull << 63);

read_only global u8 integer_symbols[16] = {
	'0',
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'A',
	'B',
	'C',
	'D',
	'E',
	'F',
};

// Includes reverses for uppercase and lowercase hex.
read_only global u8 integer_symbol_reverse[128] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
	0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

read_only global u8 base64[64] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
	'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '_', '$',
};

read_only global u8 base64_reverse[128] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
	0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E,
	0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0xFF, 0xFF, 0xFF, 0xFF,
	0x3E, 0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A,
	0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

#define g_bit(x) (1 << (x))

#define g_align_pow2(x, b) (((x) + (b) - 1) & (~((b) - 1)))
#define g_align_down_pow2(x, b) ((x) & (~((b) - 1)))
#define g_align_pad_pow2(x, b) ((0 - (x)) & ((b) - 1))
#define g_is_pow2(x) ((x) != 0 && ((x) & ((x) - 1)) == 0)
#define g_is_pow2_or_zero(x) ((((x) - 1) & (x)) == 0)

#endif // GLINT_BASE_UTIL_UTIL_H
