//hours_lost_here = 12
//я рот ебал нахуй

#include "../include/arlloc.h"

// Ебаные статики по имя инкапсуляции нахуй
static arena_block_t* arena_block_create(size_t capacity);
static void arena_block_destroy(arena_block_t* block);
static int is_power_of_two(size_t n);

arena_t* arena_create(size_t initial_block_size, size_t subsequent_block_size) {

    if (initial_block_size == 0 || subsequent_block_size == 0) {
        return NULL; 
    }

    // 1. Выделение памяти под управляющую структуру аллокатора
    arena_t* arena = (arena_t*)malloc(sizeof(arena_t));
    if (arena == NULL) {
        return NULL;
    }

    // 2. Выделение первого блока памяти
    arena_block_t* first_block = arena_block_create(initial_block_size);
    if (first_block == NULL) {
        free(arena); // Освобождаем структуру аллокатора, если блок не создан
        return NULL;
    }

    // 3. Инициализация полей аллокатора
    arena->first_block = first_block;
    arena->current_block = first_block;
    arena->subsequent_block_size = subsequent_block_size;

    return arena;
}

void arena_destroy(arena_t* arena) {
    // Защита от передачи NULL указателя (no-op поведение)
    if (arena == NULL) {
        return;
    }

    // Итерация по связному списку блоков
    arena_block_t* current_block = arena->first_block;
    
    while (current_block != NULL) {
        arena_block_t* next_block = current_block->next; // Сохраняем указатель на следующий блок
        
        // Освобождаем память текущего блока (данные + структура управления)
        arena_block_destroy(current_block);
        
        // Переходим к следующему блоку
        current_block = next_block;
    }

    // Освобождение самой управляющей структуры аллокатора
    free(arena);
}

void* arena_alloc(arena_t* arena, size_t size, size_t alignment) {
    // 1. Строгая проверка входных параметров
    if (arena == NULL) {
        return NULL;
    }
    if (size == 0) {
        return NULL; // Согласно ТЗ, для size == 0 возвращаем NULL
    }
    if (!is_power_of_two(alignment)) {
        return NULL; // Выравнивание обязано быть степенью двойки
    }

    // 2. Попытка выделения в текущем блоке
    arena_block_t* block = arena->current_block;
    
    // Вычисление выровненного абсолютного адреса
    uintptr_t base_addr = (uintptr_t)block->data;
    uintptr_t current_addr = base_addr + block->used;
    uintptr_t aligned_addr = (current_addr + alignment - 1) & ~(alignment - 1);
    size_t padding = (size_t)(aligned_addr - current_addr);
    size_t total_needed = padding + size;

    if (block->used + total_needed <= block->capacity) {
        // Места достаточно — выполняем bump
        block->used += total_needed;
        return (void*)aligned_addr;
    }

    // 3. Места в текущем блоке недостаточно — выделяем новый блок
    // Размер нового блока: максимум из subsequent_block_size и необходимого минимума.
    // Минимум = size + alignment, так как padding всегда строго меньше alignment,
    // следовательно, size + alignment гарантированно вместит запрошенную память с любым padding.
    size_t new_block_capacity = arena->subsequent_block_size;
    size_t min_required = size + alignment;
    if (new_block_capacity < min_required) {
        new_block_capacity = min_required;
    }

    arena_block_t* new_block = arena_block_create(new_block_capacity);
    if (new_block == NULL) {
        return NULL; // OOM
    }

    // 4. Добавление нового блока в связный список и обновление состояния арены
    block->next = new_block;
    arena->current_block = new_block;

    // 5. Повторное выделение в новом блоке (база нового блока может иметь иное выравнивание)
    uintptr_t new_base_addr = (uintptr_t)new_block->data;
    uintptr_t new_current_addr = new_base_addr + new_block->used; // used == 0
    uintptr_t new_aligned_addr = (new_current_addr + alignment - 1) & ~(alignment - 1);
    size_t new_padding = (size_t)(new_aligned_addr - new_current_addr);
    size_t new_total_needed = new_padding + size;

    // Эта проверка всегда должна проходить, так как new_block_capacity >= size + alignment,
    // а new_padding < alignment. Оставлена для безопасности и строгости реализации.
    if (new_block->used + new_total_needed > new_block->capacity) {
        // Теоретически недостижимо при корректной работе arena_block_create
        return NULL;
    }

    new_block->used += new_total_needed;
    return (void*)new_aligned_addr;
}

void arena_reset(arena_t* arena) {
    if (arena == NULL) {
        return;
    }

    arena_block_t* current = arena->first_block;
    if (current == NULL) {
        return; // Защита от некорректного состояния
    }

    // Освобождение всех блоков, начиная со второго
    arena_block_t* next = current->next;
    while (next != NULL) {
        arena_block_t* to_free = next;
        next = next->next;
        arena_block_destroy(to_free);
    }

    // Сброс состояния первого блока
    arena->first_block->next = NULL;
    arena->first_block->used = 0;
    arena->current_block = arena->first_block;
}

void arena_get_stats(const arena_t* arena, arena_stats_t* stats) {
    if (arena == NULL || stats == NULL) {
        return;
    }

    size_t total_allocated = 0;
    size_t total_used = 0;
    size_t block_count = 0;

    const arena_block_t* current = arena->first_block;
    while (current != NULL) {
        // Учитываем как память под данные, так и память под управляющую структуру блока
        total_allocated += current->capacity + sizeof(arena_block_t);
        total_used += current->used;
        block_count++;
        current = current->next;
    }

    stats->total_allocated_bytes = total_allocated;
    stats->total_used_bytes = total_used;
    stats->block_count = block_count;
}

//=========================================================
//++++++++++++++Ебучие статики+++++++++++++++++++++++++++++
//=========================================================

// Выделение и инициализация нового блока памяти заданного размера.
static arena_block_t* arena_block_create(size_t capacity) {
    if (capacity == 0) {
        return NULL;
    }

    arena_block_t* block = (arena_block_t*)malloc(sizeof(arena_block_t));
    if (block == NULL) {
        return NULL; // Ошибка выделения памяти под структуру управления
    }

    // Выделяем память под сами данные. 
    // malloc гарантирует выравнивание по max_align_t, что является безопасной базой 
    // для последующего bump-pointer выравнивания в arena_alloc.
    block->data = (uint8_t*)malloc(capacity);
    if (block->data == NULL) {
        free(block);
        return NULL; // Ошибка выделения памяти под данные, освобождаем структуру управления
    }

    block->next = NULL;
    block->capacity = capacity;
    block->used = 0;

    return block;
}

// Освобождение одного блока памяти (данных и структуры управления).
static void arena_block_destroy(arena_block_t* block) {
    if (block == NULL) {
        return;
    }
    
    if (block->data != NULL) {
        free(block->data);
    }
    free(block);
}

// Проверка, является ли число степенью двойки
static int is_power_of_two(size_t n) {
    return (n != 0) && ((n & (n - 1)) == 0);
}
