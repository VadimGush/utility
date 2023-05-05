//
// Created by Vadim Gush on 13.04.2023.
//

#ifndef UTILITIES_SYSTEM_H
#define UTILITIES_SYSTEM_H

#include "collections.h"
#include "types.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <chrono>

namespace sys {

    enum struct Error {
        UNKNOWN,
        ACCESS_DENIED,
        FILE_NOT_FOUND,
        FAILED_TO_READ,
    };

    /**
     * Opens the file and returns its content
     * @param path path to the file
     * @return file data if operation is completed
     */
    static result<vec<u8>, Error> read_file(const char* const path) {
        using result_t = result<vec<u8>, Error>;

        const fd_t fd = open(path, O_RDONLY);
        if (fd == -1) {
            switch (errno) {
                case EACCES: return result_t::failure(Error::ACCESS_DENIED);
                case ENOENT: return result_t::failure(Error::FILE_NOT_FOUND);
                default: return result_t::failure(Error::UNKNOWN);
            }
        }
        struct stat stat{};
        fstat(fd, &stat);
        size_t file_size = stat.st_size;

        vec<u8> data(file_size);
        size_t bytes_read = read(fd, data.data(), file_size);
        if (bytes_read != file_size) {
            return result_t::failure(Error::FAILED_TO_READ);
        }
        close(fd);
        return result_t::success(std::move(data));
    };


    /**
     * Measures time.
     */
    template <typename T = f32, typename S = std::micro>
    struct clock {
        using duration = std::chrono::duration<T, S>;
        using clock_type = std::chrono::high_resolution_clock;

        std::chrono::time_point<clock_type, duration> start_{};
        std::chrono::time_point<clock_type, duration> end_{};

        clock() { start_ = clock_type::now(); }
        void complete() { end_ = clock_type::now(); }

        T milli() const { return std::chrono::duration_cast<std::chrono::duration<T, std::milli>>(end_ - start_).count(); }
        T micro() const { return std::chrono::duration_cast<std::chrono::duration<T, std::micro>>(end_ - start_).count(); }
        T nano() const { return std::chrono::duration_cast<std::chrono::duration<T, std::nano>>(end_ - start_).count(); }

        T ms() const { return milli(); }
        T ns() const { return nano(); }
    };

}

#endif //UTILITIES_SYSTEM_H
