#include <check.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "../arlloc.h"
#include "main_tests.h"

// Тест: выделение валидного размера возвращает не-NULL.
START_TEST(test_alloc_valid)
{
    arena_t* arena = arena_create(4096, 4096);
    void* ptr = arena_alloc(arena, 128, 8);
    ck_assert_ptr_nonnull(ptr);
    arena_destroy(arena);
}
END_TEST

// Тест: size == 0 возвращает NULL.
START_TEST(test_alloc_zero_size)
{
    arena_t* arena = arena_create(4096, 4096);
    void* ptr = arena_alloc(arena, 0, 8);
    ck_assert_ptr_null(ptr);
    arena_destroy(arena);
}
END_TEST

// Тест: NULL arena возвращает NULL.
START_TEST(test_alloc_null_arena)
{
    void* ptr = arena_alloc(NULL, 128, 8);
    ck_assert_ptr_null(ptr);
}
END_TEST

// Тест: alignment не являющийся степенью двойки возвращает NULL.
START_TEST(test_alloc_invalid_alignment)
{
    arena_t* arena = arena_create(4096, 4096);
    void* ptr = arena_alloc(arena, 128, 3); // 3 не степень двойки
    ck_assert_ptr_null(ptr);
    ptr = arena_alloc(arena, 128, 0);       // 0 недопустимо
    ck_assert_ptr_null(ptr);
    arena_destroy(arena);
}
END_TEST

// Тест: возвращаемый указатель строго выровнен по запрошенной границе.
START_TEST(test_alloc_alignment_correctness)
{
    arena_t* arena = arena_create(4096, 4096);

    size_t alignments[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
    size_t n = sizeof(alignments) / sizeof(alignments[0]);

    for (size_t i = 0; i < n; i++) {
        size_t align = alignments[i];
        void* ptr = arena_alloc(arena, 37, align); // намеренно не кратный размер
        ck_assert_ptr_nonnull(ptr);
        ck_assert_uint_eq(((uintptr_t)ptr) % align, 0);
    }

    arena_destroy(arena);
}
END_TEST

// Тест: выделенная память доступна для чтения и записи.
START_TEST(test_alloc_memory_accessible)
{
    arena_t* arena = arena_create(4096, 4096);
    uint8_t* ptr = (uint8_t*)arena_alloc(arena, 256, 8);
    ck_assert_ptr_nonnull(ptr);

    memset(ptr, 0xAB, 256);
    for (int i = 0; i < 256; i++) {
        ck_assert_uint_eq(ptr[i], 0xAB);
    }

    arena_destroy(arena);
}
END_TEST

// Тест: при переполнении первого блока аллокатор переходит к следующему блоку.
START_TEST(test_alloc_overflow_to_new_block)
{
    arena_t* arena = arena_create(64, 1024);
    ck_assert_ptr_nonnull(arena);

    // Заполняем первый блок (64 байта)
    void* p1 = arena_alloc(arena, 32, 8);
    void* p2 = arena_alloc(arena, 32, 8);
    ck_assert_ptr_nonnull(p1);
    ck_assert_ptr_nonnull(p2);

    // Следующая аллокация должна перейти в новый блок
    void* p3 = arena_alloc(arena, 128, 8);
    ck_assert_ptr_nonnull(p3);

    // Указатели из разных блоков не должны перекрываться
    ck_assert((uintptr_t)p3 < (uintptr_t)p1 || (uintptr_t)p3 > (uintptr_t)p2 + 32);

    arena_destroy(arena);
}
END_TEST

// Тест: аллокация размера, превышающего subsequent_block_size, всё равно succeeds.
START_TEST(test_alloc_larger_than_subsequent)
{
    arena_t* arena = arena_create(64, 64);
    void* ptr = arena_alloc(arena, 1024, 8); // > subsequent_block_size
    ck_assert_ptr_nonnull(ptr);
    memset(ptr, 0, 1024); // проверка доступности
    arena_destroy(arena);
}
END_TEST

Suite* suite_alloc(void)
{
    Suite* s = suite_create("Alloc");
    TCase* tc = tcase_create("Core");

    tcase_add_test(tc, test_alloc_valid);
    tcase_add_test(tc, test_alloc_zero_size);
    tcase_add_test(tc, test_alloc_null_arena);
    tcase_add_test(tc, test_alloc_invalid_alignment);
    tcase_add_test(tc, test_alloc_alignment_correctness);
    tcase_add_test(tc, test_alloc_memory_accessible);
    tcase_add_test(tc, test_alloc_overflow_to_new_block);
    tcase_add_test(tc, test_alloc_larger_than_subsequent);

    suite_add_tcase(s, tc);
    return s;
}
