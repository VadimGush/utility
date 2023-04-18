//
// Created by Vadim Gush on 13.04.2023.
//
#include <gtest/gtest.h>
#include "../include/utils/unix.h"
#include <fcntl.h>

void create_test_file(const char* const filename, const str& content) {
    fd_t test_file = open(filename, O_CREAT | O_WRONLY, S_IRWXU | S_IRWXO | S_IRWXG);
    write(test_file, content.c_str(), content.size());
    close(test_file);
}

void delete_test_file(const char* const filename) {
    remove(filename);
}

TEST(UnixTest, read_file_returns_success) {
    str actual_content = "Hello world";
    create_test_file("file.txt", actual_content);

    const result<vec<u8>, unix::Error> result_data = unix::read_file("file.txt");
    ASSERT_TRUE(result_data);
    const auto& data = result_data.value();
    str content(reinterpret_cast<const char*>(data.data()), data.size());
    ASSERT_EQ(content, actual_content);

    delete_test_file("file.txt");
}

TEST(UnixTest, read_file_returns_file_not_found) {
    const result<vec<u8>, unix::Error> result_data = unix::read_file("file.txt");
    ASSERT_FALSE(result_data);
    const auto& failure = result_data.failure();
    ASSERT_EQ(unix::Error::FILE_NOT_FOUND, failure);
}
