//
// Created by Vadim Gush on 13.04.2023.
//

#ifndef UTILITIES_UNIX_H
#define UTILITIES_UNIX_H

#include "collections.h"
#include "types.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

namespace unix {

    enum struct Result {
        SUCCESS = 0,
        FAILURE = 1,
        ACCESS_DENIED = 2,
        FILE_NOT_FOUND = 3,
        FAILED_TO_READ = 4,
    };

    /**
     * Opens the file and returns its content
     * @param path path to the file
     * @param result if operation is successful Result::SUCCESS
     * @return file data if operation is completed
     */
    static vec<u8> read_file(IN const char* const path, OUT Result* result) {
        const fd_t fd = open(path, 0);
        if (fd == -1) {
            switch (errno) {
                case EACCES: { *result = Result::ACCESS_DENIED; break; }
                case ENOENT: { *result = Result::FILE_NOT_FOUND; break; }
                default: *result = Result::FAILURE;
            }
            return {};
        }
        struct stat stat{};
        fstat(fd, &stat);
        size_t file_size = stat.st_size;

        vec<u8> data(file_size);
        size_t bytes_read = read(fd, data.data(), file_size);
        if (bytes_read != file_size) {
            *result = Result::FAILED_TO_READ;
            return {};
        }
        *result = Result::SUCCESS;
        return data;
    };

}

#endif //UTILITIES_UNIX_H
