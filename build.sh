#!/bin/bash
# Glint Build Script for Linux/macOS

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
CC="${CC:-gcc}"
# Use -std=c23 for strict C23, or -std=gnu23 for GNU extensions
# Set GLINT_USE_GNU=1 to enable GNU extensions
if [ "${GLINT_USE_GNU:-0}" = "1" ]; then
    STD_FLAG="-std=gnu23"
else
    STD_FLAG="-std=c23"
fi
CFLAGS="${STD_FLAG} -Wall -Wextra -D_GNU_SOURCE -Ibase -Itests"
BUILD_DIR="build"
TEST_DIR="tests"

# Platform detection
if [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM="macos"
else
    PLATFORM="linux"
fi

echo -e "${BLUE}=== Glint Build System ===${NC}"
echo -e "${BLUE}Platform: $PLATFORM${NC}"
echo -e "${BLUE}Compiler: $CC${NC}"
echo ""

# Create build directory
mkdir -p "$BUILD_DIR"
mkdir -p "$BUILD_DIR/tests"

# Function to print section headers
print_section() {
    echo -e "\n${YELLOW}>>> $1${NC}"
}

# Function to compile a test
compile_test() {
    local test_name=$1
    local test_path="$TEST_DIR/$test_name"
    local output="$BUILD_DIR/tests/$test_name"

    echo -e "${BLUE}Compiling $test_name...${NC}"

    if [ -f "$test_path/${test_name}.c" ]; then
        # Compile (suppress warnings for cleaner output)
        $CC $CFLAGS \
            "$TEST_DIR/g_test_main.c" \
            "$test_path/${test_name}.c" \
            -o "$output" \
            -lm 2>&1 > /dev/null

        # Check if binary was created
        if [ -f "$output" ]; then
            echo -e "${GREEN}✓ $test_name compiled successfully${NC}"
            return 0
        else
            echo -e "${RED}✗ $test_name compilation failed${NC}"
            # Recompile with output to show errors
            $CC $CFLAGS \
                "$TEST_DIR/g_test_main.c" \
                "$test_path/${test_name}.c" \
                -o "$output" \
                -lm
            return 1
        fi
    else
        echo -e "${RED}✗ Test file not found: $test_path/${test_name}.c${NC}"
        return 1
    fi
}

# Function to run a test
run_test() {
    local test_name=$1
    local test_binary="$BUILD_DIR/tests/$test_name"

    if [ ! -f "$test_binary" ]; then
        echo -e "${RED}✗ Test binary not found: $test_binary${NC}"
        return 1
    fi

    echo -e "${BLUE}Running $test_name...${NC}"

    if "$test_binary" "$2"; then
        echo -e "${GREEN}✓ $test_name passed${NC}"
        return 0
    else
        echo -e "${RED}✗ $test_name failed${NC}"
        return 1
    fi
}

# Build command
build_all() {
    print_section "Building Tests"

    local failed=0

    # Find all test directories
    for test_dir in "$TEST_DIR"/g_test_*/; do
        if [ -d "$test_dir" ]; then
            test_name=$(basename "$test_dir")
            compile_test "$test_name" || ((failed++))
        fi
    done

    echo ""
    if [ $failed -eq 0 ]; then
        echo -e "${GREEN}All tests compiled successfully!${NC}"
        return 0
    else
        echo -e "${RED}$failed test(s) failed to compile${NC}"
        return 1
    fi
}

# Test command
run_all_tests() {
    print_section "Running Tests"

    local failed=0
    local passed=0

    for test_binary in "$BUILD_DIR/tests"/g_test_*; do
        if [ -f "$test_binary" ] && [ -x "$test_binary" ]; then
            test_name=$(basename "$test_binary")
            echo ""
            if run_test "$test_name" "$1"; then
                ((passed++))
            else
                ((failed++))
            fi
        fi
    done

    echo ""
    echo -e "${BLUE}=== Test Summary ===${NC}"
    echo -e "${GREEN}Passed: $passed${NC}"
    echo -e "${RED}Failed: $failed${NC}"

    if [ $failed -eq 0 ]; then
        echo -e "\n${GREEN}All tests passed! ✓${NC}"
        return 0
    else
        echo -e "\n${RED}Some tests failed ✗${NC}"
        return 1
    fi
}

# Clean command
clean() {
    print_section "Cleaning Build Directory"
    rm -rf "$BUILD_DIR"
    echo -e "${GREEN}Build directory cleaned${NC}"
}

# Help command
show_help() {
    echo "Usage: ./build.sh [command] [options]"
    echo ""
    echo "Commands:"
    echo "  build          Compile all tests (default)"
    echo "  test           Run all tests"
    echo "  all            Build and run all tests"
    echo "  clean          Remove build directory"
    echo "  help           Show this help message"
    echo ""
    echo "Options:"
    echo "  [filter]       When running tests, filter by name"
    echo ""
    echo "Environment Variables:"
    echo "  CC             C compiler (default: gcc)"
    echo "  GLINT_USE_GNU  Use GNU C23 extensions (default: 0, strict C23)"
    echo ""
    echo "Examples:"
    echo "  ./build.sh build"
    echo "  ./build.sh test"
    echo "  ./build.sh test math           # Run only tests matching 'math'"
    echo "  ./build.sh all"
    echo "  CC=clang ./build.sh build"
    echo "  GLINT_USE_GNU=1 ./build.sh all # Use GNU extensions"
    echo ""
    echo "See STANDARDS.md for information about C23 vs GNU C23"
}

# Main script logic
case "${1:-build}" in
    build)
        build_all
        ;;
    test)
        run_all_tests "$2"
        ;;
    all)
        build_all && run_all_tests "$2"
        ;;
    clean)
        clean
        ;;
    help|--help|-h)
        show_help
        ;;
    *)
        echo -e "${RED}Unknown command: $1${NC}"
        echo ""
        show_help
        exit 1
        ;;
esac
