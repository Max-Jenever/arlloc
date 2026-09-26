#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

// Внутренняя структура блока памяти. Не экспортируется в заголовочный файл.
typedef struct arena_block_t {
    struct arena_block_t* next; // Указатель на следующий блок в связном списке
    size_t capacity;            // Общий размер блока data в байтах
    size_t used;                // Текущее смещение (offset) от начала data
    uint8_t* data;              // Указатель на фактически выделенную память для данных
} arena_block_t;

// Внутренняя структура самого аллокатора. Определяет typedef arena_t из arena.h.
struct arena_t {
    arena_block_t* current_block;   // Блок, в котором в данный момент происходит выделение
    arena_block_t* first_block;     // Первый (initial) блок, сохраняется для arena_reset
    size_t subsequent_block_size;   // Размер всех последующих блоков при переполнении
};
// Создание аллокатора
arena_t* arena_create(size_t initial_block_size, size_t subsequent_block_size);

// Уничтожение аллокатора и освобождение всей памяти в ОС
void arena_destroy(arena_t* arena);

// Выделение памяти
void* arena_alloc(arena_t* arena, size_t size, size_t alignment);

// Сброс логического состояния аллокатора (освобождение всех блоков, кроме первого)
void arena_reset(arena_t* arena);

// Получение статистики
void arena_get_stats(const arena_t* arena, arena_stats_t* stats);

#ifdef __cplusplus
}
#endif

#endif // ARENA_H
