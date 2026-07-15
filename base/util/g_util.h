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

global const u32 g_bitmask1	 = 0x00000001;
global const u32 g_bitmask2	 = 0x00000003;
global const u32 g_bitmask3	 = 0x00000007;
global const u32 g_bitmask4	 = 0x0000000f;
global const u32 g_bitmask5	 = 0x0000001f;
global const u32 g_bitmask6	 = 0x0000003f;
global const u32 g_bitmask7	 = 0x0000007f;
global const u32 g_bitmask8	 = 0x000000ff;
global const u32 g_bitmask9	 = 0x000001ff;
global const u32 g_bitmask10 = 0x000003ff;
global const u32 g_bitmask11 = 0x000007ff;
global const u32 g_bitmask12 = 0x00000fff;
global const u32 g_bitmask13 = 0x00001fff;
global const u32 g_bitmask14 = 0x00003fff;
global const u32 g_bitmask15 = 0x00007fff;
global const u32 g_bitmask16 = 0x0000ffff;
global const u32 g_bitmask17 = 0x0001ffff;
global const u32 g_bitmask18 = 0x0003ffff;
global const u32 g_bitmask19 = 0x0007ffff;
global const u32 g_bitmask20 = 0x000fffff;
global const u32 g_bitmask21 = 0x001fffff;
global const u32 g_bitmask22 = 0x003fffff;
global const u32 g_bitmask23 = 0x007fffff;
global const u32 g_bitmask24 = 0x00ffffff;
global const u32 g_bitmask25 = 0x01ffffff;
global const u32 g_bitmask26 = 0x03ffffff;
global const u32 g_bitmask27 = 0x07ffffff;
global const u32 g_bitmask28 = 0x0fffffff;
global const u32 g_bitmask29 = 0x1fffffff;
global const u32 g_bitmask30 = 0x3fffffff;
global const u32 g_bitmask31 = 0x7fffffff;
global const u32 g_bitmask32 = 0xffffffff;

global const u64 g_bitmask33 = 0x00000001ffffffffull;
global const u64 g_bitmask34 = 0x00000003ffffffffull;
global const u64 g_bitmask35 = 0x00000007ffffffffull;
global const u64 g_bitmask36 = 0x0000000fffffffffull;
global const u64 g_bitmask37 = 0x0000001fffffffffull;
global const u64 g_bitmask38 = 0x0000003fffffffffull;
global const u64 g_bitmask39 = 0x0000007fffffffffull;
global const u64 g_bitmask40 = 0x000000ffffffffffull;
global const u64 g_bitmask41 = 0x000001ffffffffffull;
global const u64 g_bitmask42 = 0x000003ffffffffffull;
global const u64 g_bitmask43 = 0x000007ffffffffffull;
global const u64 g_bitmask44 = 0x00000fffffffffffull;
global const u64 g_bitmask45 = 0x00001fffffffffffull;
global const u64 g_bitmask46 = 0x00003fffffffffffull;
global const u64 g_bitmask47 = 0x00007fffffffffffull;
global const u64 g_bitmask48 = 0x0000ffffffffffffull;
global const u64 g_bitmask49 = 0x0001ffffffffffffull;
global const u64 g_bitmask50 = 0x0003ffffffffffffull;
global const u64 g_bitmask51 = 0x0007ffffffffffffull;
global const u64 g_bitmask52 = 0x000fffffffffffffull;
global const u64 g_bitmask53 = 0x001fffffffffffffull;
global const u64 g_bitmask54 = 0x003fffffffffffffull;
global const u64 g_bitmask55 = 0x007fffffffffffffull;
global const u64 g_bitmask56 = 0x00ffffffffffffffull;
global const u64 g_bitmask57 = 0x01ffffffffffffffull;
global const u64 g_bitmask58 = 0x03ffffffffffffffull;
global const u64 g_bitmask59 = 0x07ffffffffffffffull;
global const u64 g_bitmask60 = 0x0fffffffffffffffull;
global const u64 g_bitmask61 = 0x1fffffffffffffffull;
global const u64 g_bitmask62 = 0x3fffffffffffffffull;
global const u64 g_bitmask63 = 0x7fffffffffffffffull;
global const u64 g_bitmask64 = 0xffffffffffffffffull;

global const u32 g_bit1	 = (1 << 0);
global const u32 g_bit2	 = (1 << 1);
global const u32 g_bit3	 = (1 << 2);
global const u32 g_bit4	 = (1 << 3);
global const u32 g_bit5	 = (1 << 4);
global const u32 g_bit6	 = (1 << 5);
global const u32 g_bit7	 = (1 << 6);
global const u32 g_bit8	 = (1 << 7);
global const u32 g_bit9	 = (1 << 8);
global const u32 g_bit10 = (1 << 9);
global const u32 g_bit11 = (1 << 10);
global const u32 g_bit12 = (1 << 11);
global const u32 g_bit13 = (1 << 12);
global const u32 g_bit14 = (1 << 13);
global const u32 g_bit15 = (1 << 14);
global const u32 g_bit16 = (1 << 15);
global const u32 g_bit17 = (1 << 16);
global const u32 g_bit18 = (1 << 17);
global const u32 g_bit19 = (1 << 18);
global const u32 g_bit20 = (1 << 19);
global const u32 g_bit21 = (1 << 20);
global const u32 g_bit22 = (1 << 21);
global const u32 g_bit23 = (1 << 22);
global const u32 g_bit24 = (1 << 23);
global const u32 g_bit25 = (1 << 24);
global const u32 g_bit26 = (1 << 25);
global const u32 g_bit27 = (1 << 26);
global const u32 g_bit28 = (1 << 27);
global const u32 g_bit29 = (1 << 28);
global const u32 g_bit30 = (1 << 29);
global const u32 g_bit31 = (1 << 30);
global const u32 g_bit32 = (1 << 31);

global const u64 g_bit33 = (1ull << 32);
global const u64 g_bit34 = (1ull << 33);
global const u64 g_bit35 = (1ull << 34);
global const u64 g_bit36 = (1ull << 35);
global const u64 g_bit37 = (1ull << 36);
global const u64 g_bit38 = (1ull << 37);
global const u64 g_bit39 = (1ull << 38);
global const u64 g_bit40 = (1ull << 39);
global const u64 g_bit41 = (1ull << 40);
global const u64 g_bit42 = (1ull << 41);
global const u64 g_bit43 = (1ull << 42);
global const u64 g_bit44 = (1ull << 43);
global const u64 g_bit45 = (1ull << 44);
global const u64 g_bit46 = (1ull << 45);
global const u64 g_bit47 = (1ull << 46);
global const u64 g_bit48 = (1ull << 47);
global const u64 g_bit49 = (1ull << 48);
global const u64 g_bit50 = (1ull << 49);
global const u64 g_bit51 = (1ull << 50);
global const u64 g_bit52 = (1ull << 51);
global const u64 g_bit53 = (1ull << 52);
global const u64 g_bit54 = (1ull << 53);
global const u64 g_bit55 = (1ull << 54);
global const u64 g_bit56 = (1ull << 55);
global const u64 g_bit57 = (1ull << 56);
global const u64 g_bit58 = (1ull << 57);
global const u64 g_bit59 = (1ull << 58);
global const u64 g_bit60 = (1ull << 59);
global const u64 g_bit61 = (1ull << 60);
global const u64 g_bit62 = (1ull << 61);
global const u64 g_bit63 = (1ull << 62);
global const u64 g_bit64 = (1ull << 63);

read_only global u8 g_integer_symbols[16] = {
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
read_only global u8 g_integer_symbol_reverse[128] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
	0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

read_only global u8 g_base64[64] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
	'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '_', '$',
};

read_only global u8 g_base64_reverse[128] = {
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
