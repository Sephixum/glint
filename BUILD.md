# Glint Build System

Cross-platform build and test system for the Glint project.

## Quick Start

### Linux/macOS

```bash
# Build all tests
./build.sh build

# Run all tests
./build.sh test

# Build and run tests
./build.sh all

# Clean build directory
./build.sh clean
```

### Windows (Command Prompt)

```cmd
REM Build all tests
build.bat build

REM Run all tests
build.bat test

REM Build and run tests
build.bat all

REM Clean build directory
build.bat clean
```



## Commands

### build
Compiles all test modules found in the `tests/` directory.

```bash
./build.sh build
```

Each test is compiled as a separate executable linking with `g_test_main.c`.

### test
Runs all compiled tests.

```bash
./build.sh test
```

You can filter tests by name:

```bash
# Linux/macOS
./build.sh test math        # Only run tests with 'math' in the name

# Windows
build.bat test math         # Only run tests with 'math' in the name
```

### all
Builds and runs all tests in one command.

```bash
./build.sh all
```

### clean
Removes the `build/` directory and all compiled binaries.

```bash
./build.sh clean
```

## Project Structure

```
glint/
├── base/                   # Core library code
│   ├── core/
│   ├── math/
│   ├── memory/
│   ├── string/
│   └── util/
├── tests/                  # Test suite
│   ├── g_test_common.h    # Test framework
│   ├── g_test_main.c      # Test runner
│   ├── g_test_arena/      # Arena tests
│   ├── g_test_enum_to_string/
│   ├── g_test_math/       # Math library tests
│   └── g_test_string/     # String tests
├── build/                  # Build output (generated)
│   └── tests/             # Compiled test binaries
├── build.sh               # Unix build script
└── build.bat              # Windows batch script
```

## Test Modules

### g_test_math
Tests for the math library (`base/math/`):
- Vector operations (vec2, vec3, vec4)
- Matrix operations (mat4x4)
- Quaternion operations
- Math constants and conversions

### g_test_string
Tests for the string library (`base/string/`):
- Character utilities
- C-string operations
- String creation and manipulation
- String matching
- Numeric conversions

### g_test_arena
Tests for the arena allocator (`base/memory/`):
- Basic allocation
- Temporary arenas
- Scratch arenas (thread-local)
- Conflict resolution
- Arena clearing

### g_test_enum_to_string
Tests for enum-to-string conversions:
- Operating system enum
- Arena flags
- String flags
- Path style flags

## Compiler Configuration

### Default Compiler
- Linux/macOS: `gcc`
- Windows: `gcc` (MinGW)

### Compiler Flags
- `-std=c23` - Use C23 standard
- `-Wall -Wextra` - Enable warnings
- `-I.` - Include current directory
- `-lm` - Link math library

### Custom Compiler

#### Linux/macOS
```bash
CC=clang ./build.sh build
```

#### Windows
```cmd
set CC=clang
build.bat build
```

## Test Framework

Tests use the `GLINT_TEST_*` macros from `g_test_common.h`:

```c
#include "../g_test_common.h"

static void test_example(void)
{
    GLINT_TEST_SECTION("example section");
    
    GLINT_TEST_CHECK(1 + 1 == 2);
    GLINT_TEST_CHECK_EQ_INT(5, 5);
    GLINT_TEST_CHECK_EQ_FLOAT(3.14f, 3.14f, 0.001f);
}

static void register_example_tests(void)
{
    GLINT_TEST_REGISTER(test_example);
}

#include "../../base/implementation.c"
```

## Adding New Tests

1. Create a new directory in `tests/`: `tests/g_test_mymodule/`
2. Create the test file: `tests/g_test_mymodule/g_test_mymodule.c`
3. Follow the test framework conventions (see existing tests)
4. Include implementation files at the end
5. Run `./build.sh all` to build and test

The build system will automatically discover and compile your test.

## Troubleshooting

### Permission Denied (Linux/macOS)
Make the script executable:
```bash
chmod +x build.sh
```

### Compiler Not Found
Make sure GCC or your chosen compiler is in your PATH:
```bash
# Linux/macOS
which gcc

# Windows
where gcc
```

### Test Compilation Fails
Check that:
- All required source files are present
- Header includes are correct
- Implementation files are included at the end of test files

## Continuous Integration

The build scripts return proper exit codes:
- `0` - Success
- `1` - Failure

This makes them suitable for CI/CD pipelines:

```yaml
# Example GitHub Actions
- name: Build and Test
  run: ./build.sh all
```

## Platform Support

| Platform | Script | Status |
|----------|--------|--------|
| Linux    | `build.sh` | ✅ Tested |
| macOS    | `build.sh` | ✅ Supported |
| Windows  | `build.bat` | ✅ Supported |

## License

Same as the Glint project.
