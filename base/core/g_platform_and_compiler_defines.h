#ifndef GLINT_BASE_CORE_PLATFORM_AND_COMPILER_DEFINES_H
#define GLINT_BASE_CORE_PLATFORM_AND_COMPILER_DEFINES_H

// ============================================================================
// Language Standard Detection
// ============================================================================

// Check for GNU extensions
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#	define GLINT_GNU_EXTENSIONS 1
#else
#	define GLINT_GNU_EXTENSIONS 0
#endif

// Check for POSIX
#if defined(_POSIX_VERSION)
#	define GLINT_POSIX 1
#else
#	define GLINT_POSIX 0
#endif

// Feature test: determine if we have POSIX or need to use alternatives
#if defined(_POSIX_VERSION) && _POSIX_VERSION >= 200112L
#	define GLINT_HAS_SYSCONF 1
#else
#	define GLINT_HAS_SYSCONF 0
#endif

// ============================================================================
// Compiler Detection
// ============================================================================

#if defined(__clang__)
#	define GLINT_COMPILER_CLANG 1
#	if defined(_WIN32)
#		define GLINT_OS_WINDOWS 1
#	elif defined(__gnu_linux__) || defined(__linux__)
#		define GLINT_OS_LINUX 1
#	elif defined(__APPLE__) && defined(__MACH__)
#		define GLINT_OS_MAC 1
#	else
#		error "This compiler/OS combo is not supported."
#	endif

#	if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#		define GLINT_ARCH_X64 1
#	elif defined(i386) || defined(__i386) || defined(__i386__)
#		define GLINT_ARCH_X86 1
#	elif defined(__aarch64__)
#		define GLINT_ARCH_ARM64 1
#	elif defined(__arm__)
#		define GLINT_ARCH_ARM32 1
#	else
#		error "Architecture not supported."
#	endif

#elif defined(_MSC_VER)
#	define GLINT_COMPILER_MSVC 1

#	if _MSC_VER >= 1920
#		define GLINT_COMPILER_MSVC_YEAR 2019
#	elif _MSC_VER >= 1910
#		define GLINT_COMPILER_MSVC_YEAR 2017
#	elif _MSC_VER >= 1900
#		define GLINT_COMPILER_MSVC_YEAR 2015
#	elif _MSC_VER >= 1800
#		define GLINT_COMPILER_MSVC_YEAR 2013
#	elif _MSC_VER >= 1700
#		define GLINT_COMPILER_MSVC_YEAR 2012
#	elif _MSC_VER >= 1600
#		define GLINT_COMPILER_MSVC_YEAR 2010
#	elif _MSC_VER >= 1500
#		define GLINT_COMPILER_MSVC_YEAR 2008
#	elif _MSC_VER >= 1400
#		define GLINT_COMPILER_MSVC_YEAR 2005
#	else
#		define GLINT_COMPILER_MSVC_YEAR 0
#	endif

#	if defined(_WIN32)
#		define GLINT_OS_WINDOWS 1
#	else
#		error "This compiler/OS combo is not supported."
#	endif

#	if defined(_M_AMD64)
#		define GLINT_ARCH_X64 1
#	elif defined(_M_IX86)
#		define GLINT_ARCH_X86 1
#	elif defined(_M_ARM64)
#		define GLINT_ARCH_ARM64 1
#	elif defined(_M_ARM)
#		define GLINT_ARCH_ARM32 1
#	else
#		error "Architecture not supported."
#	endif

#elif defined(__GNUC__) || defined(__GNUG__)
#	define GLINT_COMPILER_GCC 1

#	if defined(__gnu_linux__) || defined(__linux__)
#		define GLINT_OS_LINUX 1
#	elif defined(_WIN32) || defined(_WIN64)
#		define GLINT_OS_WINDOWS 1
#	else
#		error "This compiler/OS combo is not supported."
#	endif

#	if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#		define GLINT_ARCH_X64 1
#	elif defined(i386) || defined(__i386) || defined(__i386__)
#		define GLINT_ARCH_X86 1
#	elif defined(__aarch64__)
#		define GLINT_ARCH_ARM64 1
#	elif defined(__arm__)
#		define GLINT_ARCH_ARM32 1
#	else
#		error "Architecture not supported."
#	endif

#else
#	error "Compiler not supported."

#endif

#if defined(GLINT_ARCH_X64)
#	define GLINT_ARCH_64BIT 1
#elif defined(GLINT_ARCH_X86)
#	define GLINT_ARCH_32BIT 1
#endif

#if GLINT_ARCH_ARM32 || GLINT_ARCH_ARM64 || GLINT_ARCH_X64 || GLINT_ARCH_X86
#	define GLINT_ARCH_LITTLE_ENDIAN 1
#else
#	error "Endianness of this architecture not understood by context cracker."
#endif

#if defined(__cplusplus)
#	define GLINT_LANG_CPP 1
#else
#	define GLINT_LANG_C 1
#endif

#if !defined(GLINT_BUILD_DEBUG)
#	define GLINT_BUILD_DEBUG 1
#endif

#if !defined(GLINT_BUILD_VERSION_MAJOR)
#	define GLINT_BUILD_VERSION_MAJOR 0
#endif

#if !defined(GLINT_BUILD_VERSION_MINOR)
#	define GLINT_BUILD_VERSION_MINOR 0
#endif

#if !defined(GLINT_BUILD_VERSION_PATCH)
#	define GLINT_BUILD_VERSION_PATCH 1
#endif

#define GLINT_BUILD_VERSION_STRING_LITERAL                                                                             \
	Stringify(GLINT_BUILD_VERSION_MAJOR) "." Stringify(GLINT_BUILD_VERSION_MINOR) "." Stringify(                       \
		GLINT_BUILD_VERSION_PATCH)

#if GLINT_BUILD_DEBUG
#	define GLINT_BUILD_MODE_STRING_LITERAL_APPEND " [Debug]"
#else
#	define GLINT_BUILD_MODE_STRING_LITERAL_APPEND ""
#endif

#if !defined(GLINT_ARCH_32BIT)
#	define GLINT_ARCH_32BIT 0
#endif
#if !defined(GLINT_ARCH_64BIT)
#	define GLINT_ARCH_64BIT 0
#endif
#if !defined(GLINT_ARCH_X64)
#	define GLINT_ARCH_X64 0
#endif
#if !defined(GLINT_ARCH_X86)
#	define GLINT_ARCH_X86 0
#endif
#if !defined(GLINT_ARCH_ARM64)
#	define GLINT_ARCH_ARM64 0
#endif
#if !defined(GLINT_ARCH_ARM32)
#	define GLINT_ARCH_ARM32 0
#endif
#if !defined(GLINT_COMPILER_MSVC)
#	define GLINT_COMPILER_MSVC 0
#endif
#if !defined(GLINT_COMPILER_GCC)
#	define GLINT_COMPILER_GCC 0
#endif
#if !defined(GLINT_COMPILER_CLANG)
#	define GLINT_COMPILER_CLANG 0
#endif
#if !defined(GLINT_OS_WINDOWS)
#	define GLINT_OS_WINDOWS 0
#endif
#if !defined(GLINT_OS_LINUX)
#	define GLINT_OS_LINUX 0
#endif
#if !defined(GLINT_OS_MAC)
#	define GLINT_OS_MAC 0
#endif
#if !defined(GLINT_LANG_CPP)
#	define GLINT_LANG_CPP 0
#endif
#if !defined(GLINT_LANG_C)
#	define GLINT_LANG_C 0
#endif

#if GLINT_ARCH_X86
#	error "You tried to build in x86 (32 bit) mode, but currently, only building in x64 (64 bit) mode is supported."
#endif

#if !GLINT_ARCH_X64
#	error "You tried to build with an unsupported architecture. Currently, only building in x64 mode is supported."
#endif

#if GLINT_OS_WINDOWS
#	if (GLITN_COMPILER_GCC || GLINT_COMPILER_CLANG)
#		define GLINT_DLL_EXPORT __attribute__((dllexport))
#		define GLINT_DLL_IMPORT __attribute__((dllimport))
#	else
#		define GLINT_DLL_EXPORT __declspec(dllexport)
#		define GLINT_DLL_IMPORT __declspec(dllimport)
#	endif
#elif GLINT_OS_LINUX || GLINT_OS_MAC
#	define GLINT_DLL_EXPORT __attribute__((visibility("default")))
#	define GLINT_DLL_IMPORT
#else
#	define GLINT_DLL_EXPORT
#	define GLINT_DLL_IMPORT
#endif

#if GLINT_OS_WINDOWS
#	define shared_function C_LINKAGE __declspec(dllexport)
#else
#	define shared_function C_LINKAGE
#endif

#if GLINT_LANG_CPP
#	define GLINT_C_LINKAGE_BEGIN                                                                                      \
		extern "C"                                                                                                     \
		{
#	define GLINT_C_LINKAGE_END }
#	define GLINT_C_LINKAGE extern "C"
#else
#	define GLINT_C_LINKAGE_BEGIN
#	define GLINT_C_LINKAGE_END
#	define GLINT_C_LINKAGE
#endif

#if GLINT_COMPILER_MSVC || (GLINT_COMPILER_CLANG && GLINT_OS_WINDOWS)
#	pragma section(".rdata$", read)
#	define read_only __declspec(allocate(".rdata$"))
#elif (GLINT_COMPILER_CLANG && GLINT_OS_LINUX)
#	define read_only __attribute__((section(".rodata")))
#else
#	define read_only
#endif

#if GLINT_COMPILER_MSVC
#	define force_inline __forceinline
#elif GLINT_COMPILER_CLANG || GLINT_COMPILER_GCC
#	define force_inline __attribute__((always_inline))
#else
#	error "force_inline not defined for this compiler."
#endif

#if GLINT_COMPILER_MSVC
#	define no_inline __declspec(noinline)
#elif GLINT_COMPILER_CLANG || GLINT_COMPILER_GCC
#	define no_inline __attribute__((noinline))
#else
#	error "no_inline not defined for this compiler."
#endif

#if GLINT_COMPILER_MSVC
#	define g_trap() __debugbreak()
#elif GLINT_COMPILER_CLANG || GLINT_COMPILER_GCC
#	define g_trap() __builtin_trap()
#else
#	error "Unknown trap intrinsic for this compiler."
#endif

#define g_assert_always(x)                                                                                             \
	do                                                                                                                 \
	{                                                                                                                  \
		if (!(x))                                                                                                      \
		{                                                                                                              \
			g_trap();                                                                                                  \
		}                                                                                                              \
	} while (0)

#if GLINT_BUILD_DEBUG
#	define g_assert(x) g_assert_always(x)
#else
#	define g_assert(x) (void)(x)
#endif

#define g_invalid_path g_assert(!"Invalid Path!")
#define g_not_implemented g_assert(!"Not Implemented!")
#define g_no_op ((void)0)
#define g_static_assert(C, ID) global u8 g_glue(ID, __LINE__)[(C) ? 1 : -1]

#if GLINT_ARCH_64BIT
#	define g_int_from_ptr(ptr) ((u64)(ptr))
#elif GLINT_ARCH_32BIT
#	define g_int_from_ptr(ptr) ((u32)(ptr))
#else
#	error "Missing pointer-to-integer cast for this architecture."
#endif

typedef enum operating_system
{
	operating_system_null,
	operating_system_windows,
	operating_system_linux,
	operating_system_mac,
	operating_system_COUNT,
#if GLINT_OS_WINDOWS
	operating_system_current = operating_system_windows,
#elif GLINT_OS_LINUX
	operating_system_current = operating_system_linux,
#elif GLINT_OS_MAC
	operating_system_current = operating_system_max,
#else
	operating_system_current = operating_system_null,
#endif
} operating_system;

#endif // GLINT_BASE_CORE_PLATFORM_AND_COMPILER_DEFINES_H
