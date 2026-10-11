#ifndef ARLLOC_H
#define ARLLOC_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Непрозрачный тип для-handle аллокатора
typedef struct arena_t arena_t;

// Структура для получения статистики аллокатора (опционально, но рекомендуется для профилирования)
typedef struct arena_stats_t {
    size_t total_allocated_bytes;   // Общий объем памяти, запрошенный у ОС
    size_t total_used_bytes;        // Общий объем памяти, фактически выделенный пользователю (с учетом padding)
    size_t block_count;             // Количество выделенных блоков
} arena_stats_t;

arena_t* arena_create(size_t initial_block_size, size_t subsequent_block_size);
void arena_destroy(arena_t* arena);
void* arena_alloc(arena_t* arena, size_t size, size_t alignment);
void arena_reset(arena_t* arena);
void arena_get_stats(const arena_t* arena, arena_stats_t* stats);

#ifdef __cplusplus
}
#endif

#endif // ARLLOC_H
