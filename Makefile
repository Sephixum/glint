# Glint Makefile
# Simple alternative to build scripts

.PHONY: all build test clean help

# Configuration
CC ?= gcc
# Use -std=c23 for strict C23, or -std=gnu23 for GNU extensions
# Set GLINT_USE_GNU=1 to enable: make GLINT_USE_GNU=1
ifeq ($(GLINT_USE_GNU),1)
	STD_FLAG = -std=gnu23
else
	STD_FLAG = -std=c23
endif
CFLAGS = $(STD_FLAG) -Wall -Wextra -I.
BUILD_DIR = build
TEST_DIR = tests

# Colors
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[1;33m
BLUE = \033[0;34m
NC = \033[0m

# Find all test directories
TEST_DIRS := $(wildcard $(TEST_DIR)/g_test_*)
TEST_NAMES := $(notdir $(TEST_DIRS))
TEST_BINS := $(addprefix $(BUILD_DIR)/tests/, $(TEST_NAMES))

# Default target
all: build test

# Build all tests
build: $(TEST_BINS)

# Pattern rule to build each test
$(BUILD_DIR)/tests/g_test_%: $(TEST_DIR)/g_test_%/g_test_%.c $(TEST_DIR)/g_test_main.c | $(BUILD_DIR)/tests
	@echo "$(BLUE)Compiling g_test_$*...$(NC)"
	@$(CC) $(CFLAGS) $^ -o $@ -lm 2>&1 | head -20 || true
	@if [ -f $@ ]; then \
		echo "$(GREEN)✓ g_test_$* compiled successfully$(NC)"; \
	else \
		echo "$(RED)✗ g_test_$* compilation failed$(NC)"; \
		exit 1; \
	fi

# Create build directories
$(BUILD_DIR)/tests:
	@mkdir -p $(BUILD_DIR)/tests

# Run all tests
test: $(TEST_BINS)
	@echo ""
	@echo "$(YELLOW)>>> Running Tests$(NC)"
	@echo ""
	@PASSED=0; \
	FAILED=0; \
	for test in $(TEST_BINS); do \
		if [ -f $$test ] && [ -x $$test ]; then \
			echo ""; \
			echo "$(BLUE)Running $$(basename $$test)...$(NC)"; \
			if $$test $(FILTER); then \
				echo "$(GREEN)✓ $$(basename $$test) passed$(NC)"; \
				PASSED=$$((PASSED + 1)); \
			else \
				echo "$(RED)✗ $$(basename $$test) failed$(NC)"; \
				FAILED=$$((FAILED + 1)); \
			fi; \
		fi; \
	done; \
	echo ""; \
	echo "$(BLUE)=== Test Summary ===$(NC)"; \
	echo "$(GREEN)Passed: $$PASSED$(NC)"; \
	echo "$(RED)Failed: $$FAILED$(NC)"; \
	if [ $$FAILED -eq 0 ]; then \
		echo ""; \
		echo "$(GREEN)All tests passed! ✓$(NC)"; \
		exit 0; \
	else \
		echo ""; \
		echo "$(RED)Some tests failed ✗$(NC)"; \
		exit 1; \
	fi

# Clean build directory
clean:
	@echo "$(YELLOW)>>> Cleaning Build Directory$(NC)"
	@rm -rf $(BUILD_DIR)
	@echo "$(GREEN)Build directory cleaned$(NC)"

# Help
help:
	@echo "Glint Makefile"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all            Build and run all tests (default)"
	@echo "  build          Compile all tests"
	@echo "  test           Run all tests"
	@echo "  clean          Remove build directory"
	@echo "  help           Show this help message"
	@echo ""
	@echo "Variables:"
	@echo "  CC             C compiler (default: gcc)"
	@echo "  FILTER         Test name filter for 'make test'"
	@echo ""
	@echo "Examples:"
	@echo "  make build"
	@echo "  make test"
	@echo "  make test FILTER=math      # Run only tests matching 'math'"
	@echo "  make clean"
	@echo "  CC=clang make build"
