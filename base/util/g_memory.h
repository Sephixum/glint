#ifndef GLINT_BASE_MEMORY_MEMORY_UTIL_H
#define GLINT_BASE_MEMORY_MEMORY_UTIL_H

#define g_memory_copy(dst, src, size) memmove((dst), (src), (size))
#define g_memory_set(dst, byte, size) memset((dst), (byte), (size))
#define g_memory_compare(a, b, size) memcmp((a), (b), (size))

#define g_memory_copy_struct(d, s) g_memory_copy((d), (s), sizeof(*(d)))
#define g_memory_copy_array(d, s) g_memory_copy((d), (s), sizeof(d))
#define g_memory_copy_typed(d, s, c) g_memory_copy((d), (s), sizeof(*(d)) * (c))
#define g_memory_copy_string8(dst, s) g_memory_copy(dst, (s).str, (s).size)

#define g_memory_zero(s, z) memset((s), 0, (z))
#define g_memory_zero_struct(s) g_memory_zero((s), sizeof(*(s)))
#define g_memory_zero_array(a) g_memory_zero((a), sizeof(a))
#define g_memory_zero_typed(m, c) g_memory_zero((m), sizeof(*(m)) * (c))

#define g_memory_match(a, b, z) (g_memory_compare((a), (b), (z)) == 0)
#define g_memory_match_struct(a, b) g_memory_match((a), (b), sizeof(*(a)))
#define g_memory_match_array(a, b) g_memory_match((a), (b), sizeof(a))

#endif // GLINT_BASE_MEMORY_MEMORY_UTIL_H
