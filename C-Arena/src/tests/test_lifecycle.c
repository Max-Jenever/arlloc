#include <check.h>
#include <stddef.h>
#include "../arlloc.h"
#include "main_tests.h"

// Тест: создание арены с валидными параметрами возвращает не-NULL.
START_TEST(test_create_valid)
{
    arena_t* arena = arena_create(1024, 2048);
    ck_assert_ptr_nonnull(arena);
    arena_destroy(arena);
}
END_TEST

// Тест: создание арены с нулевым initial_block_size возвращает NULL.
START_TEST(test_create_zero_initial)
{
    arena_t* arena = arena_create(0, 2048);
    ck_assert_ptr_null(arena);
}
END_TEST

// Тест: создание арены с нулевым subsequent_block_size возвращает NULL.
START_TEST(test_create_zero_subsequent)
{
    arena_t* arena = arena_create(1024, 0);
    ck_assert_ptr_null(arena);
}
END_TEST

// Тест: arena_destroy с NULL не вызывает падения (no-op).
START_TEST(test_destroy_null)
{
    arena_destroy(NULL); // не должно быть segfault
    ck_assert(1);
}
END_TEST

// Тест: множественное создание и уничтожение не приводит к утечкам (проверяется внешними инструментами, здесь — отсутствие падения).
START_TEST(test_create_destroy_multiple)
{
    for (int i = 0; i < 100; i++) {
        arena_t* arena = arena_create(512, 512);
        ck_assert_ptr_nonnull(arena);
        arena_destroy(arena);
    }
}
END_TEST

Suite* suite_lifecycle(void)
{
    Suite* s = suite_create("Lifecycle");
    TCase* tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_create_valid);
    tcase_add_test(tc_core, test_create_zero_initial);
    tcase_add_test(tc_core, test_create_zero_subsequent);
    tcase_add_test(tc_core, test_destroy_null);
    tcase_add_test(tc_core, test_create_destroy_multiple);

    suite_add_tcase(s, tc_core);
    return s;
}
