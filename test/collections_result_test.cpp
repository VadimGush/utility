//
// Created by Vadim Gush on 13.04.2023.
//
#include <gtest/gtest.h>
#include "../include/utils/collections.h"

TEST(ResultTest, result_returns_true_on_success) {
    auto r = result<int, int>::success(1);
    ASSERT_TRUE(r);
}

TEST(ResultTest, result_returns_false_on_failure) {
    auto r = result<int, int>::failure(1);
    ASSERT_FALSE(r);
}

TEST(ResultTest, result_returns_correct_value_on_success) {
    auto r = result<int, int>::success(7);
    ASSERT_EQ(r.value(), 7);
}

TEST(ResultTest, result_returns_correct_value_on_failure) {
    auto r = result<int, int>::failure(13);
    ASSERT_EQ(r.failure(), 13);
}

TEST(ResultTest, result_throws_exception_on_failure) {
    auto r = result<int, int>::failure(13);
    ASSERT_ANY_THROW(r.value());
}

TEST(ResultTest, result_throws_exception_on_success) {
    auto r = result<int, int>::success(7);
    ASSERT_ANY_THROW(r.failure());
}



