//
// Created by Vadim Gush on 13.04.2023.
//
#include <gtest/gtest.h>
#include "../include/utils/collections.h"

TEST(ResultTest, ResultReturnsTrueOnSuccess) {
    auto r = result<int, int>::success(1);
    ASSERT_TRUE(r);
}

TEST(ResultTest, ResultReturnsFalseOnFailure) {
    auto r = result<int, int>::failure(1);
    ASSERT_FALSE(r);
}

TEST(ResultTest, ResultReturnsCorrectValueOnSuccess) {
    auto r = result<int, int>::success(7);
    ASSERT_EQ(r.value(), 7);
}

TEST(ResultTest, ResultReturnsCorrectValueOnFailure) {
    auto r = result<int, int>::failure(13);
    ASSERT_EQ(r.failure(), 13);
}

TEST(ResultTest, ResultThrowsExceptionOnFailure) {
    auto r = result<int, int>::failure(13);
    ASSERT_ANY_THROW(r.value());
}

TEST(ResultTest, ResultThrowsExceptionOnSuccess) {
    auto r = result<int, int>::success(7);
    ASSERT_ANY_THROW(r.failure());
}



