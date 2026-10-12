use std::ffi::c_void;
use std::ptr;

#[allow(non_camel_case_types)]
pub enum arena_t {}

#[allow(clippy::missing_safety_doc)]
unsafe extern "C" {
    fn arena_create(initial_block_size: usize, subsequent_block_size: usize) -> *mut arena_t;
    fn arena_destroy(arena: *mut arena_t);
    fn arena_alloc(arena: *mut arena_t, size: usize, align: usize) -> *mut c_void; // + align
    fn arena_reset(arena: *mut arena_t);
}

/// Обертка над C-аллокатором арены.
///
/// # Ограничения
/// Метод `alloc` поддерживает только типы, реализующие типаж `Copy`.
/// Аллокатор арены не выполняет вызов деструкторов (`Drop`) для размещенных данных.
pub struct Arena {
    ptr: *mut arena_t,
}

impl Arena {
    /// Создает новый экземпляр аллокатора арены.
    ///
    /// # Параметры
    /// * `initial_size` — размер первого блока памяти в байтах (должен быть > 0).
    /// * `subsequent_size` — размер последующих блоков памяти в байтах (должен быть > 0).
    ///
    /// # Возвращаемое значение
    /// Возвращает `Some(Arena)` при успешном выделении памяти или `None` в случае ошибки (OOM).
    pub fn new(initial_size: usize, subsequent_size: usize) -> Option<Self> {
        let ptr = unsafe { arena_create(initial_size, subsequent_size) };
        if ptr.is_null() {
            None
        } else {
            Some(Self { ptr })
        }
    }
    /// Выделяет память в арене и размещает в ней переданное значение.
    ///
    /// # Параметры
    /// * `value` — значение для размещения. Тип `T` обязан реализовывать типаж `Copy`.
    ///
    /// # Возвращаемое значение
    /// Возвращает `Some(&mut T)` с изменяемой ссылкой на размещенное значение, 
    /// время жизни которой привязано к времени жизни самой арены (`'a`). 
    /// Возвращает `None` в случае исчерпания памяти или невалидных параметров.
    ///
    /// # Гарантии безопасности
    /// Функция является безопасной (safe), так как внутренне гарантирует корректное 
    /// вычисление размера и выравнивания через `std::mem::size_of` и `std::mem::align_of`.
    pub fn alloc<'a, T>(&'a self, value: T) -> Option<&'a mut T> {
        let size = std::mem::size_of::<T>();
        let align = std::mem::align_of::<T>();

        let raw_ptr = unsafe { arena_alloc(self.ptr, size, align) };

        if raw_ptr.is_null() {
            return None;
        }

        unsafe {
            let typed_ptr = raw_ptr as *mut T;
            ptr::write(typed_ptr, value);
            Some(&mut *typed_ptr)
        }
    }

    /// Сбрасывает арену в начальное состояние.
    ///
    /// # Внимание
    /// Все ссылки, полученные ранее через метод `alloc`, становятся невалидными. 
    /// Использование таких ссылок после вызова `reset` приведет к неопределенному поведению (Undefined Behavior).
    pub fn reset(&self) {
        unsafe { arena_reset(self.ptr) };
    }
}

impl Drop for Arena {
    /// Освобождает все ресурсы, связанные с ареной, при выходе из области видимости.
    fn drop(&mut self) {
        if !self.ptr.is_null() {
            unsafe { arena_destroy(self.ptr) };
        }
    }
}

//=========================================================
// Tests
//=========================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_arena_creation_and_alloc() {
        let arena = Arena::new(4096, 4096).expect("Failed to create Arena");

        let val = arena.alloc(42_u32).expect("Failed to alloc val"); // ← val создаётся здесь
        assert_eq!(*val, 42);

        #[derive(Debug, PartialEq)]
        struct Point { x: f64, y: f64 }

        let point = arena.alloc(Point { x: 1.0, y: 1.0 }).expect("Failed to alloc point");
        assert_eq!(*point, Point { x: 1.0, y: 1.0 });
    }

    #[test]
    fn test_arena_reset() {
        let arena = Arena::new(1024, 1024).unwrap();

        let _val = arena.alloc(100_u64).unwrap();

        arena.reset();

        let new_val = arena.alloc(200_u64).unwrap();
        assert_eq!(*new_val, 200);
    }
}
