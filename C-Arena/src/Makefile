CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -pedantic
GCOV_FLAGS = -fprofile-arcs -ftest-coverage
CHECK_CFLAGS := $(shell pkg-config --cflags check 2>/dev/null)
CHECK_LIBS := $(shell pkg-config --libs check 2>/dev/null)
SYSTEM_NAME := $(shell uname -s)

ifeq ($(strip $(CHECK_LIBS)),)
CHECK_LIBS = -lcheck -lm -lsubunit -lrt -pthread
endif

SRC = $(wildcard *.c)

TEST_SRC = $(wildcard tests/*.c tests/*.h)

# Превращаем все .c в .o
OBJ = $(SRC:.c=.o)
TEST_OBJ = $(TEST_SRC:.c=.o)
GCOV_OBJ = $(SRC:.c=.gcov.o)
GCOV_TEST_OBJ = $(TEST_SRC:.c=.gcov.o)

# Целевая директория и файлы
BUILD_DIR = ../build
TARGET = $(BUILD_DIR)/libarlloc.a
TEST_RUNNER = test_runner
GCOV_TEST_RUNNER = test_runner_gcov

all: $(TARGET)

# Пересборка с нуля
rebuild: clean all

# Гарантируем существование директории сборки
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Сборка статической библиотеки (с зависимостью от существования директории)
$(TARGET): $(OBJ) | $(BUILD_DIR)
	ar rcs $@ $^
	ranlib $@

# Правило для компиляции .c в .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.gcov.o: %.c
	$(CC) $(CFLAGS) $(GCOV_FLAGS) -c $< -o $@

# Запуск тестов
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
	       $(BUILD_DIR)/libarlloc.a

.PHONY: all test gcov_report clean cppchk rebuild
