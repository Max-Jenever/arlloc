#include <check.h>
#include <stddef.h>
#include "../arlloc.h"
#include "main_tests.h"

//Шлюха ебаная нахуй

// Тест: начальная статистика после создания арены.
START_TEST(test_stats_initial)
{
    arena_t* arena = arena_create(1024, 2048);
    arena_stats_t stats;
    arena_get_stats(arena, &stats);

    ck_assert_uint_eq(stats.block_count, 1);
    ck_assert_uint_eq(stats.total_used_bytes, 0);
    // total_allocated_bytes >= 1024 (размер первого блока) + sizeof(arena_block_t)
    ck_assert_uint_ge(stats.total_allocated_bytes, 1024);

    arena_destroy(arena);
}
END_TEST

// Тест: статистика корректно отражает несколько аллокаций.
START_TEST(test_stats_after_allocs)
{
    arena_t* arena = arena_create(4096, 4096);

    arena_alloc(arena, 100, 8);
    arena_alloc(arena, 200, 8);
    arena_alloc(arena, 300, 8);

    arena_stats_t stats;
    arena_get_stats(arena, &stats);

    ck_assert_uint_eq(stats.block_count, 1);
    ck_assert_uint_ge(stats.total_used_bytes, 600); // минимум 100+200+300

    arena_destroy(arena);
}
END_TEST

// Тест: статистика учитывает padding для выравнивания.
START_TEST(test_stats_accounts_padding)
{
    arena_t* arena = arena_create(4096, 4096);

    // Первая аллокация: 33 байта с выравниванием 1 (без padding)
    arena_alloc(arena, 33, 1);
    
    // Вторая аллокация: 1 байт с выравниванием 64
    // Так как 33 не кратно 64, текущий адрес не выровнен по 64,
    // следовательно, будет padding минимум 31 байт (до следующего кратного 64)
    arena_alloc(arena, 1, 64);

    arena_stats_t stats;
    arena_get_stats(arena, &stats);

    // total_used_bytes должен быть >= 33 + 31 (padding) + 1 = 65
    // Проверяем, что он строго больше, чем просто сумма размеров (33 + 1 = 34)
    ck_assert_uint_gt(stats.total_used_bytes, 34);

    arena_destroy(arena);
}
END_TEST

// Тест: arena_get_stats с NULL не вызывает падения.
START_TEST(test_stats_null_args)
{
    arena_t* arena = arena_create(1024, 1024);
    arena_stats_t stats;

    arena_get_stats(NULL, &stats);  // no-op
    arena_get_stats(arena, NULL);   // no-op
    arena_get_stats(NULL, NULL);    // no-op

    ck_assert(1);
    arena_destroy(arena);
}
END_TEST

Suite* suite_stats(void)
{
    Suite* s = suite_create("Stats");
    TCase* tc = tcase_create("Core");

    tcase_add_test(tc, test_stats_initial);
    tcase_add_test(tc, test_stats_after_allocs);
    tcase_add_test(tc, test_stats_accounts_padding);
    tcase_add_test(tc, test_stats_null_args);

    suite_add_tcase(s, tc);
    return s;
}
