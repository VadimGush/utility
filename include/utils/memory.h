#ifndef UTILITIES_MEMORY_H
#define UTILITIES_MEMORY_H

#include "types.h"
#include "markers.h"

namespace allocator {

    struct stack {

        u8* data = nullptr;
        // The id of the last byte of data
        size_t head = 0;
        // Total available size in bytes
        size_t size = 0;

        enum struct status {
            SUCCESS,
            NOT_ENOUGH_SPACE,
        };

        stack() {} 

        stack(void* const buffer, const size_t buffer_size) {
            data = (u8*)buffer;
            size = buffer_size;
        }

        status allocate(IN const size_t n, OUT void** pointer) {
            if (head + n > size) {
                return status::NOT_ENOUGH_SPACE;
            }
            head += n;
            *pointer = data + head;
            return status::SUCCESS;
        }

        // Free some data from the end of the stack
        void free(const size_t n) {
            head -= n;
        }

        bool is_empty() {
            return head == 0;
        }

        void clear() {
            head = 0;
        }

        // Removes data from the beggining of the stack
        // this will end up performing full copy
        void clear_start(const size_t bytes_to_remove) {
            head -= bytes_to_remove;
            for (size_t i = bytes_to_remove; i < size; i++) {
                data[i - bytes_to_remove] = data[i];
            }
        }

    };

}

#endif //UTILITIES_MEMORY_H
