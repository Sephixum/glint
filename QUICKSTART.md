# Glint Quick Start Guide

Get up and running with the Glint project in minutes.

## Installation

### Prerequisites

**All Platforms:**
- GCC or Clang compiler with C23 support
- Git (for cloning)

**Linux:**
```bash
sudo apt install build-essential  # Debian/Ubuntu
sudo dnf install gcc make         # Fedora/RHEL
```

**macOS:**
```bash
xcode-select --install  # Install command line tools
```

**Windows:**
- Install [MinGW-w64](https://www.mingw-w64.org/) or [MSYS2](https://www.msys2.org/)
- Or use [Visual Studio](https://visualstudio.microsoft.com/) with C compiler

## Quick Build

### Linux/macOS

```bash
# Clone and build
git clone <repository-url> glint
cd glint

# Build and test
./build.sh all
```

### Windows

```cmd
REM Clone and build
git clone <repository-url> glint
cd glint

REM Build and test
build.bat all
```

## Build Methods

Choose your preferred build method:

### 1. Build Scripts (Recommended)

**Most flexible, works everywhere**

```bash
# Linux/macOS
./build.sh all

# Windows
build.bat all
```

### 2. Makefile

**Quick and simple for Unix-like systems**

```bash
make all
```

### 3. Manual Compilation

**For understanding or custom setups**

```bash
gcc -std=c23 -Wall -Wextra -I. \
    tests/g_test_math/g_test_math.c \
    tests/g_test_main.c \
    -o build/test_math \
    -lm
```

## Running Tests

### All Tests

```bash
# Linux/macOS
./build.sh test

# Windows
build.bat test

# Make
make test
```

### Filtered Tests

Run only specific tests:

```bash
# Linux/macOS
./build.sh test math        # Only math tests

# Windows
build.bat test string       # Only string tests

# Make
make test FILTER=arena
```

## Project Structure

```
glint/
├── base/                    # Core libraries
│   ├── core/               # Platform and types
│   ├── math/               # Math library (vectors, matrices, quaternions)
│   ├── memory/             # Arena allocator
│   ├── string/             # String utilities
│   └── util/               # Utilities
│
├── tests/                   # Test suite
│   ├── g_test_arena/       # Memory allocator tests
│   ├── g_test_math/        # Math library tests
│   ├── g_test_string/      # String tests
│   └── g_test_enum_to_string/
│
├── build.sh                 # Unix build script
├── build.bat                # Windows batch script
├── Makefile                 # GNU Make file
├── BUILD.md                 # Detailed build documentation
└── QUICKSTART.md           # This file
```

## Using the Libraries

### Math Library

```c
#include "base/math/g_math.h"

// Vectors
vec3_f32 v1 = vec3_f32_create(1.0f, 2.0f, 3.0f);
vec3_f32 v2 = vec3_f32_create(4.0f, 5.0f, 6.0f);
vec3_f32 sum = vec3_f32_add(v1, v2);
f32 dot = vec3_f32_dot(v1, v2);

// Matrices
mat4x4_f32 identity = mat4x4_f32_create_identity();
mat4x4_f32 translation = mat4x4_f32_translate(vec3_f32_create(10, 0, 0));
mat4x4_f32 result = mat4x4_f32_mul(identity, translation);

// Quaternions
vec3_f32 axis = vec3_f32_create(0, 1, 0);
f32 angle = GLINT_DEG2RAD_F32(45.0f);
quat4_f32 rotation = quat4_f32_from_axis_angle(axis, angle);
```

### Arena Allocator

```c
#include "base/memory/g_arena.h"

// Create arena
arena* a = arena_create_default(.name = "my_arena");

// Allocate
u64* number = arena_push_struct(a, u64);
u64* array = arena_push_array(a, u64, 100);

// Temporary allocations
temp_arena temp = temp_arena_begin(a);
u64* temp_data = arena_push_array(a, u64, 50);
// ... use temp_data ...
temp_arena_end(temp);  // Free temp_data

// Thread-local scratch arenas
temp_arena scratch = arena_get_scratch(&a, 1);  // Avoid conflict with 'a'
// ... use scratch.arena ...
arena_release_scratch(scratch);

// Cleanup
arena_destroy(a);
```

### String Library

```c
#include "base/string/g_string.h"

arena* a = arena_create_default();

// String creation
string str1 = string_lit_comp("hello");
string str2 = string_from_cstr("world");

// String operations
string concatenated = string_concat(a, str1, str2);
b8 matches = string_match(str1, str2, 0);

// Numeric conversions
u64 num = u64_from_string(string_lit_comp("12345"), 10);
string num_str = string_from_u64(a, 42, 10, 0, 0);

// Formatting
string formatted = string_f(a, "Hello %s, number: %d", "World", 42);

arena_destroy(a);
```

## Common Tasks

### Add a New Test

1. Create directory:
```bash
mkdir tests/g_test_mymodule
```

2. Create test file `tests/g_test_mymodule/g_test_mymodule.c`:
```c
#include "../g_test_common.h"
#include "../../base/mymodule/mymodule.h"

static void test_mymodule_feature(void)
{
    GLINT_TEST_SECTION("mymodule feature");
    GLINT_TEST_CHECK(1 + 1 == 2);
}

static void register_mymodule_tests(void)
{
    GLINT_TEST_REGISTER(test_mymodule_feature);
}

#include "../../base/mymodule/mymodule.c"
```

3. Build and test:
```bash
./build.sh all
```

### Change Compiler

```bash
# Linux/macOS
CC=clang ./build.sh build

# Windows
set CC=clang
build.bat build

# Make
make CC=clang build
```

### Debug Build

Add debug flags:

```bash
# Linux/macOS
CFLAGS="-g -O0 -DDEBUG" ./build.sh build

# Make
make CFLAGS="-std=c23 -g -O0 -DDEBUG -I." build
```

## Troubleshooting

### Build Fails

1. **Check compiler:**
   ```bash
   gcc --version  # Should show C23 support
   ```

2. **Check includes:**
   Make sure implementation files are included at the end of test files

3. **Check for errors:**
   ```bash
   ./build.sh build  # Shows compilation errors
   ```

### Tests Fail

1. **Run specific test:**
   ```bash
   ./build.sh test math  # Only math tests
   ```

2. **Run test binary directly:**
   ```bash
   ./build/tests/g_test_math
   ```

3. **Check test output:**
   Tests show exactly which assertions failed

### Script Won't Run

**Linux/macOS:**
```bash
chmod +x build.sh
./build.sh help
```



## Next Steps

- Read [BUILD.md](BUILD.md) for detailed build system documentation
- Check test files in `tests/` for usage examples
- Explore library code in `base/`
- Start building your own modules!

## Getting Help

- Check [BUILD.md](BUILD.md) for build system details
- Look at existing tests for examples
- Review header files for API documentation

## Contributing

When adding features:
1. Follow the existing naming conventions
2. Add tests for new functionality
3. Ensure all tests pass: `./build.sh all`
4. Update documentation as needed

Happy coding! 🚀
