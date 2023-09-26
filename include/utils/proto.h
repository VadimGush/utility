
#ifndef PROTO_H
#define PROTO_H

#include "memory.h"

/**
Simple protocol over TCP/IP that defines the notion
of messages which can be exchanges between 2 machines over the network.
*/
namespace proto1 {

    struct connection {
        /* tcp_socket socket*/

        allocator::stack message_buffer{};

        connection(u8* const buffer, const size_t buffer_size) {
            message_buffer = allocator::stack{ buffer, buffer_size }; 
        }

        // Call clear_message() after this call, otherwise
        // the buffer for incoming messages will be full and will not be
        // able to recieve any new message from the TCP socket
        const u8* read_message() {
            return message_buffer.data;
        }

        void write_message(const u8* const message) {}

        void clear_message() {
            message_buffer.clear();
        }

        bool poll() {
            if (!message_buffer.is_empty()) {
                return true;
            }

            return false;

            // 1. Take buffer of the socket
            // 2. Scan for a complete message
            // 3. If message is present copy to current_message_buffer
            // 4. Set message_is_present = true
        }

    };

}

#endif
