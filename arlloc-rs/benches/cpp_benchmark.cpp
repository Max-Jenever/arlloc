#include <iostream>
#include <chrono>
#include <memory_resource>
#include <cstdint>

struct TestStruct {
    uint64_t a, b, c, d, e, f, g, h;
};

int main() {
    const int iterations = 10000;

    // 1. Одиночное выделение
    alignas(64) std::byte buffer_single[65536];
    std::pmr::monotonic_buffer_resource pool_single{buffer_single, sizeof(buffer_single)};
    
    auto start_single = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        void* ptr = pool_single.allocate(sizeof(TestStruct), alignof(TestStruct));
        auto* typed_ptr = static_cast<TestStruct*>(ptr);
        typed_ptr->a = 1; typed_ptr->b = 2; typed_ptr->c = 3; typed_ptr->d = 4;
        typed_ptr->e = 5; typed_ptr->f = 6; typed_ptr->g = 7; typed_ptr->h = 8;
    }
    auto end_single = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> duration_single = end_single - start_single;

    // 2. Полный жизненный цикл (имитация Rust b.iter() с созданием нового ресурса)
    // Используем стек для буфера, чтобы избежать heap-allocation внутри цикла
    auto start_lifecycle = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        alignas(64) std::byte local_buffer[65536];
        std::pmr::monotonic_buffer_resource local_pool{local_buffer, sizeof(local_buffer)};
        
        void* ptr = local_pool.allocate(sizeof(TestStruct), alignof(TestStruct));
        auto* typed_ptr = static_cast<TestStruct*>(ptr);
        typed_ptr->a = 1; typed_ptr->b = 2; typed_ptr->c = 3; typed_ptr->d = 4;
        typed_ptr->e = 5; typed_ptr->f = 6; typed_ptr->g = 7; typed_ptr->h = 8;
    }
    auto end_lifecycle = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> duration_lifecycle = end_lifecycle - start_lifecycle;

    std::cout << "C++ pmr alloc_single (x10000): " << duration_single.count() << " us (" 
              << (duration_single.count() / iterations * 1000) << " ns/alloc)\n";
    std::cout << "C++ pmr lifecycle_bulk_10k: " << duration_lifecycle.count() << " us\n";

    return 0;
}
