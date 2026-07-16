# Glint Standards and Portability Guide

## Language Standards

Glint is designed to work with both **strict C23** and **GNU C23** (with GNU extensions).

### C23 vs GNU23

#### Standard C23 (`-std=c23`)
- **Strict ISO C23** compliance
- Maximum portability
- Uses only POSIX-standard functions
- Recommended for most users

#### GNU C23 (`-std=gnu23`)
- C23 + GNU extensions
- May use GNU-specific functions (e.g., `getpagesize()`, `get_nprocs()`)
- Better performance on Linux with GCC
- Less portable to non-GNU environments

### Feature Detection

The codebase automatically detects available features:

```c
// From g_platform_and_compiler_defines.h

// GNU extensions available?
#define GLINT_GNU_EXTENSIONS 1/0

// POSIX available?
#define GLINT_POSIX 1/0

// sysconf() available?
#define GLINT_HAS_SYSCONF 1/0
```

### Writing Portable Code

Use feature macros to write code that works in both modes:

```c
// Example: Getting page size
#if GLINT_GNU_EXTENSIONS
    // Fast GNU extension
    u64 page_size = (u64)getpagesize();
#elif GLINT_HAS_SYSCONF
    // POSIX standard
    long page_size = sysconf(_SC_PAGESIZE);
#else
    // Fallback for other platforms
    u64 page_size = 4096;
#endif
```

## Building with Different Standards

### Default: Strict C23

```bash
# Linux/macOS
./build.sh build

# Windows
.\build.ps1 build

# Make
make
```

### With GNU Extensions

Enable GNU extensions for better performance on Linux:

```bash
# Linux/macOS
GLINT_USE_GNU=1 ./build.sh build

# Windows PowerShell
$env:GLINT_USE_GNU = "1"
.\build.ps1 build

# Windows CMD
set GLINT_USE_GNU=1
build.bat build

# Make
make GLINT_USE_GNU=1
```

## Platform-Specific Code

### Directory Structure

Platform-specific implementations go in `base/os/platform/`:

```
base/os/platform/
├── linux/
│   ├── g_os_memory_linux.c
│   └── g_os_system_info_linux.c
├── windows/
│   ├── g_os_memory_windows.c
│   └── g_os_system_info_windows.c
└── [other platforms]
```

### Platform Detection Macros

```c
// Operating System
#define GLINT_OS_WINDOWS  // Windows
#define GLINT_OS_LINUX    // Linux
#define GLINT_OS_MAC      // macOS

// Compiler
#define GLINT_COMPILER_CLANG  // Clang
#define GLINT_COMPILER_GCC    // GCC
#define GLINT_COMPILER_MSVC   // MSVC

// Architecture
#define GLINT_ARCH_X64    // x86-64
#define GLINT_ARCH_X86    // x86
#define GLINT_ARCH_ARM64  // ARM 64-bit
#define GLINT_ARCH_ARM32  // ARM 32-bit
```

### Example: Platform-Specific Implementation

```c
#include "../../core/g_platform_and_compiler_defines.h"

internal void platform_specific_function(void)
{
#if GLINT_OS_WINDOWS
    // Windows implementation
    // ...
#elif GLINT_OS_LINUX
    // Linux implementation
    #if GLINT_GNU_EXTENSIONS
        // Use GNU extensions for better performance
        // ...
    #else
        // Use POSIX standard functions
        // ...
    #endif
#elif GLINT_OS_MAC
    // macOS implementation
    // ...
#else
    #error "Unsupported platform"
#endif
}
```

## Standard Library Usage

### Allowed Standard Headers

**Always Safe (C23 Standard):**
- `<stdint.h>` - Fixed-width integer types
- `<stdbool.h>` - Boolean type
- `<stddef.h>` - Basic types (size_t, ptrdiff_t)
- `<string.h>` - String functions
- `<stdio.h>` - I/O functions
- `<stdlib.h>` - Memory, utilities
- `<math.h>` - Math functions
- `<time.h>` - Time functions
- `<float.h>` - Float limits

**POSIX (Linux/macOS/Unix):**
- `<unistd.h>` - Use with `#if GLINT_POSIX`
- `<pthread.h>` - Use with `#if GLINT_POSIX`

**GNU Extensions:**
- `<sys/sysinfo.h>` - Use with `#if GLINT_GNU_EXTENSIONS`

**Windows:**
- `<windows.h>` - Use with `#if GLINT_OS_WINDOWS`

### POSIX Functions

When using POSIX functions, always check for availability:

```c
#if GLINT_HAS_SYSCONF
    long result = sysconf(_SC_NPROCESSORS_ONLN);
#else
    // Fallback implementation
#endif
```

## Compiler-Specific Extensions

### GCC/Clang Attributes

```c
// Available in both GCC and Clang
#if GLINT_COMPILER_GCC || GLINT_COMPILER_CLANG
    #define GLINT_UNUSED __attribute__((unused))
    #define GLINT_NORETURN __attribute__((noreturn))
#else
    #define GLINT_UNUSED
    #define GLINT_NORETURN
#endif
```

### MSVC Specific

```c
#if GLINT_COMPILER_MSVC
    #define force_inline __forceinline
#elif GLINT_COMPILER_CLANG || GLINT_COMPILER_GCC
    #define force_inline __attribute__((always_inline))
#endif
```

## Thread-Local Storage

The project uses C23 `thread_local`:

```c
// C23 standard
thread_local int my_thread_var = 0;
```

If you need compatibility with older standards:

```c
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
    // C23
    #define thread_local thread_local
#elif defined(__GNUC__) || defined(__clang__)
    // GCC/Clang extension
    #define thread_local __thread
#elif defined(_MSC_VER)
    // MSVC
    #define thread_local __declspec(thread)
#else
    #error "thread_local not supported"
#endif
```

## Testing Portability

### Test on Multiple Compilers

```bash
# GCC with strict C23
CC=gcc ./build.sh all

# GCC with GNU extensions
CC=gcc GLINT_USE_GNU=1 ./build.sh all

# Clang with strict C23
CC=clang ./build.sh all

# Clang with GNU extensions
CC=clang GLINT_USE_GNU=1 ./build.sh all
```

### Test on Multiple Platforms

- **Linux:** Native environment
- **macOS:** Similar to Linux, BSD-based
- **Windows:** Use MinGW or MSVC
- **Cross-compile:** Use cross-compilation toolchains

## Best Practices

### 1. Prefer Standard over Extensions

```c
// Good: Use standard when possible
#if GLINT_HAS_SYSCONF
    long page_size = sysconf(_SC_PAGESIZE);
#else
    // Fallback
#endif

// Only use extensions when necessary for performance
#if GLINT_GNU_EXTENSIONS
    // GNU-specific optimization
#endif
```

### 2. Always Provide Fallbacks

```c
#if GLINT_OS_LINUX
    // Linux implementation
#elif GLINT_OS_WINDOWS
    // Windows implementation
#elif GLINT_OS_MAC
    // macOS implementation
#else
    #error "Unsupported platform - please implement for this OS"
#endif
```

### 3. Document Platform Requirements

```c
// NOTE: This function requires POSIX sysconf()
// Falls back to safe default on other platforms
internal u64 get_page_size(void)
{
    #if GLINT_HAS_SYSCONF
        // Implementation
    #else
        return 4096;  // Common page size
    #endif
}
```

### 4. Test Both Modes

Always test your code with both:
- Strict C23 (`-std=c23`)
- GNU C23 (`-std=gnu23`)

## Continuous Integration

### Recommended CI Matrix

Test on:
- **Linux:** GCC + Clang, both C23 and GNU23
- **macOS:** Clang, C23
- **Windows:** MinGW GCC, C23

```yaml
# Example GitHub Actions matrix
strategy:
  matrix:
    os: [ubuntu-latest, macos-latest, windows-latest]
    compiler: [gcc, clang]
    standard: [c23, gnu23]
    exclude:
      - os: windows-latest
        compiler: clang
      - os: macos-latest
        standard: gnu23
```

## Migration Guide

### From GNU-only to Portable

If you have existing GNU-only code:

1. **Identify GNU extensions:**
   ```bash
   grep -r "getpagesize\|get_nprocs" .
   ```

2. **Add feature checks:**
   ```c
   #if GLINT_GNU_EXTENSIONS
       // GNU code
   #else
       // Portable alternative
   #endif
   ```

3. **Test with strict C23:**
   ```bash
   ./build.sh all  # Should work without GLINT_USE_GNU
   ```

4. **Test with GNU extensions:**
   ```bash
   GLINT_USE_GNU=1 ./build.sh all
   ```

## Summary

- **Default:** Strict C23 for maximum portability
- **Optional:** GNU extensions for better Linux performance
- **Feature detection:** Automatic via macros
- **Always fallback:** Provide portable alternatives
- **Test both modes:** Ensure code works with and without extensions

For questions or issues, see the main README or BUILD.md.
