//
// Created by Vadim Gush on 15.05.2023.
//

#ifndef UTILITIES_MEMORY_H
#define UTILITIES_MEMORY_H
#include "collections.h"

namespace allocator {

    struct stack {
        vec<char> data{};
        size_t head = 0;

        stack(const size_t n): data(n) {}

        void* allocate(const size_t bytes, const size_t align) noexcept {
            const size_t offset = (align - (reinterpret_cast<std::uintptr_t>(&data[head]) % align)) % align;
            head += bytes + offset;
            return &data[head];
        }

        void deallocate(const void* ptr, const size_t bytes) noexcept {
            if (ptr == &data[head]) {
                head -= bytes;
            }
        }

        template <typename T>
        struct allocator {
            using value_type = T;
            stack& stack_allocator_;

            allocator(stack& frame_allocator): stack_allocator_(frame_allocator) {}
            allocator(const allocator& other): stack_allocator_(other.stack_allocator_) {}
            template <typename U>
            allocator(const allocator<U>& other): stack_allocator_(other.stack_allocator_) {}

            T* allocate(const size_t n) noexcept {
                return reinterpret_cast<T*>(stack_allocator_.allocate(sizeof(T) * n, alignof(T)));
            }

            void deallocate(const T* ptr, const size_t n) noexcept {
                stack_allocator_.deallocate(ptr, sizeof(T) * n);
            }
        };

        template <typename T>
        allocator<T> get() {
            return allocator<T>(*this);
        }

        void clear();
    };

}

#endif //UTILITIES_MEMORY_H
