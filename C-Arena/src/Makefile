CC = gcc

CHECK_CFLAGS = $(shell pkg-config --cflags check 2>/dev/null)
CHECK_LIBS = $(shell pkg-config --libs check 2>/dev/null || echo "-lcheck") -lm -lpthread

CFLAGS = -Wall -Wextra -Werror -std=c11 -pedantic $(CHECK_CFLAGS)

# Флаги для тестов: добавляем подавление предупреждений от заголовков библиотеки check
# -Wno-gnu-zero-variadic-macro-arguments: для Clang (macOS)
# -Wno-variadic-macros: для GCC (Ubuntu)
# -Wno-unknown-warning-option: защита от ошибок на неизвестные флаги подавления
TEST_CFLAGS = $(CFLAGS) -Wno-gnu-zero-variadic-macro-arguments -Wno-variadic-macros -Wno-unknown-warning-option

GCOV_FLAGS = -fprofile-arcs -ftest-coverage

SRC = $(wildcard *.c)
TEST_SRC = $(wildcard tests/*.c tests/*.h)

BUILD_DIR = ../build
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

# Компиляция основных файлов библиотеки (строгий режим)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Компиляция тестовых файлов (с подавлением предупреждений от check.h)
tests/%.o: tests/%.c
	$(CC) $(TEST_CFLAGS) -c $< -o $@

test: $(TEST_RUNNER)
	./$(TEST_RUNNER)

# Линкуем тесты с библиотекой
$(TEST_RUNNER): $(TEST_OBJ) $(TARGET)
	$(CC) $(CFLAGS) $(TEST_OBJ) $(TARGET) -o $@ $(CHECK_LIBS)

$(GCOV_TEST_RUNNER): $(GCOV_TEST_OBJ) $(GCOV_OBJ)
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $^ -o $@ $(CHECK_LIBS)

# Генерация отчета о покрытии
gcov_report: $(GCOV_TEST_RUNNER)
	./$(GCOV_TEST_RUNNER)
	lcov --capture --directory . --output-file coverage.info --no-external --ignore-errors mismatch
	genhtml coverage.info --output-directory gcov_report --ignore-errors source,mismatch

# Цель для запуска cppcheck
cppchk:
	cppcheck --enable=all --suppress=missingIncludeSystem --std=c11 \
	         --platform=unix64 --error-exitcode=1 --check-level=exhaustive \
	         --verbose -I. -Iutils $(SRC) $(TEST_SRC)

clean:
	rm -rf *.o *.a *.gcda *.gcno *.info $(TEST_RUNNER) $(GCOV_TEST_RUNNER) \
	       gcov_report tests/*.o tests/*.gcda tests/*.gcno \
	       $(BUILD_DIR)/libarlloc.a ../html

.PHONY: all test gcov_report clean cppchk rebuild
