CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -pedantic
GCOV_FLAGS = -fprofile-arcs -ftest-coverage

CHECK_CFLAGS := $(shell pkg-config --cflags check 2>/dev/null)
CHECK_LIBS := $(shell pkg-config --libs check 2>/dev/null)

ifeq ($(strip $(CHECK_LIBS)),)
CHECK_LIBS = -lcheck -lm -lsubunit -pthread
endif

SRC = $(wildcard *.c)
TEST_C_FILES = $(wildcard tests/*.c)
TEST_HEADERS = $(wildcard tests/*.h)
TEST_SRC = $(TEST_C_FILES) $(TEST_HEADERS)

OBJ = $(SRC:.c=.o)
TEST_OBJ = $(TEST_C_FILES:.c=.o)
GCOV_OBJ = $(SRC:.c=.gcov.o)
GCOV_TEST_OBJ = $(TEST_C_FILES:.c=.gcov.o)

BUILD_DIR = build
TARGET = $(BUILD_DIR)/libarlloc.a
TEST_RUNNER = test_runner
GCOV_TEST_RUNNER = test_runner_gcov

all: $(TARGET)

rebuild: clean all

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJ) | $(BUILD_DIR)
	ar rcs $@ $^
	ranlib $@

%.o: %.c
	$(CC) $(CFLAGS) $(CHECK_CFLAGS) -Wno-gnu-zero-variadic-macro-arguments -Wno-variadic-macros -Wno-unknown-warning-option -c $< -o $@

%.gcov.o: %.c
	$(CC) $(CFLAGS) $(CHECK_CFLAGS) -Wno-gnu-zero-variadic-macro-arguments -Wno-variadic-macros -Wno-unknown-warning-option $(GCOV_FLAGS) -c $< -o $@

test: $(TEST_RUNNER)
	./$(TEST_RUNNER)

$(TEST_RUNNER): $(TEST_OBJ) $(TARGET)
	$(CC) $(CFLAGS) $(TEST_OBJ) $(TARGET) -o $@ $(CHECK_LIBS)

$(GCOV_TEST_RUNNER): $(GCOV_TEST_OBJ) $(GCOV_OBJ)
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $^ -o $@ $(CHECK_LIBS)

gcov_report: $(GCOV_TEST_RUNNER)
	./$(GCOV_TEST_RUNNER)
	lcov --capture --directory . --output-file coverage.info --no-external --ignore-errors mismatch
	genhtml coverage.info --output-directory gcov_report --ignore-errors source,mismatch

cppchk:
	cppcheck --enable=all --suppress=missingIncludeSystem --std=c11 \
	         --platform=unix64 --error-exitcode=1 --check-level=exhaustive \
	         --verbose -I. -Iutils $(SRC) $(TEST_SRC)

clean:
	rm -rf *.o *.a *.gcda *.gcno *.info $(TEST_RUNNER) $(GCOV_TEST_RUNNER) \
	       gcov_report tests/*.o tests/*.gcda tests/*.gcno \
	       $(BUILD_DIR)/libarlloc.a ../html

.PHONY: all test gcov_report clean cppchk rebuild
