#include <check.h>
#include <stddef.h>
#include "../arlloc.h"
#include "main_tests.h"

// Тест: после reset можно снова выделять память.
START_TEST(test_reset_reuse)
{
    arena_t* arena = arena_create(256, 256);
    void* p1 = arena_alloc(arena, 128, 8);
    ck_assert_ptr_nonnull(p1);

    arena_reset(arena);

    void* p2 = arena_alloc(arena, 128, 8);
    ck_assert_ptr_nonnull(p2);

    arena_destroy(arena);
}
END_TEST

// Тест: reset с NULL не вызывает падения.
START_TEST(test_reset_null)
{
    arena_reset(NULL);
    ck_assert(1);
}
END_TEST

// Тест: после reset статистика показывает 0 использованной памяти.
START_TEST(test_reset_clears_stats)
{
    arena_t* arena = arena_create(1024, 1024);
    arena_alloc(arena, 512, 8);
    arena_alloc(arena, 256, 8);

    arena_reset(arena);

    arena_stats_t stats;
    arena_get_stats(arena, &stats);
    ck_assert_uint_eq(stats.total_used_bytes, 0);
    ck_assert_uint_eq(stats.block_count, 1); // только первый блок остался

    arena_destroy(arena);
}
END_TEST

// Тест: reset освобождает все блоки кроме первого.
START_TEST(test_reset_frees_extra_blocks)
{
    arena_t* arena = arena_create(64, 64);
    // Вызываем несколько аллокаций, чтобы создать дополнительные блоки
    for (int i = 0; i < 10; i++) {
        void* p = arena_alloc(arena, 100, 8);
        ck_assert_ptr_nonnull(p);
    }

    arena_stats_t before;
    arena_get_stats(arena, &before);
    ck_assert_uint_gt(before.block_count, 1);

    arena_reset(arena);

    arena_stats_t after;
    arena_get_stats(arena, &after);
    ck_assert_uint_eq(after.block_count, 1);

    arena_destroy(arena);
}
END_TEST

Suite* suite_reset(void)
{
    Suite* s = suite_create("Reset");
    TCase* tc = tcase_create("Core");

    tcase_add_test(tc, test_reset_reuse);
    tcase_add_test(tc, test_reset_null);
    tcase_add_test(tc, test_reset_clears_stats);
    tcase_add_test(tc, test_reset_frees_extra_blocks);

    suite_add_tcase(s, tc);
    return s;
}
