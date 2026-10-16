#ifndef ARLLOC_H
#define ARLLOC_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Непрозрачный дескриптор экземпляра аллокатора арены.
typedef struct arena_t arena_t;

/// @brief Структура, содержащая агрегированную статистику использования памяти арены.
typedef struct arena_stats_t {
    size_t total_allocated_bytes;   ///< Общий объем памяти, запрошенный у ОС (включая служебные заголовки блоков).
    size_t total_used_bytes;        ///< Объем памяти, фактически выделенный пользователю (с учетом выравнивания).
    size_t block_count;             ///< Количество активных блоков памяти в связном списке арены.
} arena_stats_t;

/// @brief Инициализирует аллокатор арены и выделяет первый блок памяти.
/// @param initial_block_size Размер первого блока в байтах. Должен быть строго > 0.
/// @param subsequent_block_size Размер всех последующих блоков в байтах. Должен быть строго > 0.
/// @return Указатель на экземпляр arena_t или NULL в случае ошибки выделения памяти (OOM) или передачи невалидных параметров.
arena_t* arena_create(size_t initial_block_size, size_t subsequent_block_size);

/// @brief Полностью освобождает все ресурсы, связанные с ареной, возвращая память операционной системе.
/// @param arena Указатель на аллокатор. Передача NULL является безопасной операцией (no-op).
/// @warning После вызова функции использование указателя arena приводит к неопределенному поведению (Undefined Behavior).
void arena_destroy(arena_t* arena);

/// @brief Выделяет непрерывный блок памяти внутри арены.
/// @param arena Указатель на аллокатор. Не должен быть NULL.
/// @param size Размер выделяемой памяти в байтах. При size == 0 возвращает NULL.
/// @param alignment Требуемое выравнивание адреса в байтах. Обязано быть степенью двойки (1, 2, 4, 8, 16, ...).
/// @return Указатель на выделенную память, адрес которого кратен alignment, или NULL при ошибке (OOM, невалидное выравнивание, size == 0).
void* arena_alloc(arena_t* arena, size_t size, size_t alignment);

/// @brief Сбрасывает арену в начальное состояние для повторного использования без возврата памяти ОС.
/// @param arena Указатель на аллокатор.
/// @details Освобождает все блоки, кроме первого, и обнуляет счетчик использованной памяти в первом блоке. 
/// Все ранее полученные указатели на данные в этой арене становятся невалидными.
void arena_reset(arena_t* arena);

/// @brief Заполняет предоставленную структуру статистикой текущего состояния арены.
/// @param arena Указатель на аллокатор. Не должен быть NULL.
/// @param stats Указатель на структуру arena_stats_t для записи данных. Не должен быть NULL.
void arena_get_stats(const arena_t* arena, arena_stats_t* stats);

#ifdef __cplusplus
}
#endif

#endif // ARLLOC_H
