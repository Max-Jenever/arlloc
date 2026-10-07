use std::ffi::c_void;
use std::ptr;

pub enum arena_t{}

// Объявление внешних С-функций
extern "C"{
    fn arena_create(initial_block_size: usize, subsequent_block_size: usize) -> *mut arena_t;

}
