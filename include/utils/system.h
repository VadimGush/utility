#ifndef UTILITIES_SYSTEM_H
#define UTILITIES_SYSTEM_H

#include "complex/collections.h"
#include "types.h"
#include "markers.h"
#include "memory.h"

#ifdef POSIX
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#endif

namespace sys {

    enum struct status {
        SUCCESS,
        ACCESS_DENIED,
        FILE_NOT_FOUND,
        CONNECTION_FAILED,
        RESOLVE_FAILED,
        UNKNOWN_ERROR,
        NOT_IMPLEMENTED,
    };
        

    namespace socket {

        struct tcp_socket {

            #ifdef POSIX
            fd_t socket_fd{};
            #endif

            allocator::stack incoming_buffer{};

            static status create(
                IN void* buffer, 
                IN const size_t buffer_size, 
                OUT tcp_socket* const socket) {

                allocator::stack incoming_buffer{ buffer, buffer_size };

                #ifdef POSIX
                const fd_t socket_fd = ::socket(AF_INET, SOCK_STREAM, 0);

                // Set to non blocking IO
                i32 status = fcntl(socket_fd, F_GETFL);
                if (status == -1) return status::UNKNOWN_ERROR;
                status |= O_NONBLOCK;
                const i32 fset_r = fcntl(socket_fd, F_SETFL, status);
                if (fset_r == -1) return status::UNKNOWN_ERROR;

                // Resolve IP address
                addrinfo* address_info = nullptr;
                const i32 getaddr_r = getaddrinfo("www.google.com", "80", NULL, &address_info);
                if (getaddr_r != 0) return status::RESOLVE_FAILED;

                // Connect to the IP address
                const i32 connect_r = connect(
                    socket_fd, 
                    address_info->ai_addr, 
                    address_info->ai_addrlen);
                if (connect_r != 0) return status::CONNECTION_FAILED;

                freeaddrinfo(address_info);

                socket->socket_fd = socket_fd;
                socket->incoming_buffer = incoming_buffer;
                return status::SUCCESS;

                #else // POSIX

                return status::NOT_IMPLEMENTED;

                #endif
            }

            // Call clear() after this call, otherwise incoming_buffer in the tcp_socket
            // will become full and will be unable to recieve new data
            size_t read(OUT u8** buffer) {
                *buffer = incoming_buffer.data;
                return incoming_buffer.head;
            }

            void clear(const size_t bytes_read) {
                incoming_buffer.clear_start(bytes_read);
            }

            void write(const void* const data) {}

            bool poll() {
                #ifdef POSIX

                // make buffer for incoming data
                void* read_buffer = nullptr;
                incoming_buffer.allocate(1 * KB, &read_buffer);

                // Read data
                const size_t bytes_read = ::read(socket_fd, &read_buffer, 1 * KB);
                if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                    // clear all allocated space
                    incoming_buffer.free(1 * KB);
                } else if (bytes_read != -1) {
                    // clear free space
                    incoming_buffer.free(1 * KB - bytes_read);
                } else {
                    // some other error happened
                    incoming_buffer.free(1 * KB);
                }
                #endif

                // 1. Check if TCP socket has any data
                // 2. If it does, copy it to the incoming_buffer
                return !incoming_buffer.is_empty();
            }

            ~tcp_socket() {
                #ifdef POSIX
                close(socket_fd);
                #endif
            }

        };

    }


    namespace file {
    
        static status process_error() {
            #ifdef POSIX
            switch (errno) {
                case EACCES: return status::ACCESS_DENIED;
                case ENOENT: return status::FILE_NOT_FOUND;
            }
            return status::UNKNOWN_ERROR;
            #else
            return status::NOT_IMPLEMENTED;
            #endif
        }

        static status get_size(IN const char* const path, OUT size_t* size) {
            #ifdef POSIX
            struct stat stat_buf{};
            const int status = stat(path, &stat_buf);
            if (status != 0) {
                return process_error();
            }

            *size = stat_buf.st_size;
            return status::SUCCESS;
            #else
            return status::NOT_IMPLEMENTED;
            #endif
        }

        static status read(IN const char* const path, IN size_t bytes_to_read, OUT void* const data) {
            #ifdef POSIX
            const fd_t fd = open(path, O_RDONLY);
            if (fd == -1) {
                return process_error();
            }

            const size_t bytes_read = ::read(fd, data, bytes_to_read);
            if (bytes_read != bytes_to_read) {
                return status::UNKNOWN_ERROR;
            }
            close(fd);

            return status::SUCCESS;
            #else
            return status::NOT_IMPLEMENTED;
            #endif
        }

    }

}

#endif //UTILITIES_SYSTEM_H
