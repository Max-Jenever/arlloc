use std::ffi::c_void;
use std::ptr;

pub enum ArenaT {}   // было arena_t — CamelCase

// Объявление внешних С-функций
unsafe extern "C" {
    fn arena_create(initial_block_size: usize, subsequent_block_size: usize) -> *mut ArenaT;
    fn arena_destroy(arena: *mut ArenaT);
    fn arena_alloc(arena: *mut ArenaT, size: usize, align: usize) -> *mut c_void; // + align
    fn arena_reset(arena: *mut ArenaT);
}

pub struct Arena {
    ptr: *mut ArenaT,
}

impl Arena {
    pub fn new(initial_size: usize, subsequent_size: usize) -> Option<Self> {
        let ptr = unsafe { arena_create(initial_size, subsequent_size) };
        if ptr.is_null() {
            None
        } else {
            Some(Self { ptr })
        }
    }

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

    pub fn reset(&self) {
        unsafe { arena_reset(self.ptr) };
    }
}

impl Drop for Arena {
    fn drop(&mut self) {
        if !self.ptr.is_null() {
            unsafe { arena_destroy(self.ptr) };
            self.ptr = ptr::null_mut();
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