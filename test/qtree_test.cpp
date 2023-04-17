//
// Created by Vadim Gush on 17.04.2023.
//
#include <gtest/gtest.h>
#include "../include/utils/qtree.h"

TEST(QtreeTest, BasicTest) {
    vec<vec2> points{
        vec2{25, 25},
        vec2{75, 75},
        vec2{25, 75},
        vec2{75, 25},
    };
    const auto inside_check = [](const vec2& point, const vec2& start, const vec2& end){
        return point.x >= start.x && point.x < end.x && point.y >= start.y && point.y < end.y;
    };
    const qtree<vec2> tree = qtree<vec2>::build(points, vec2{100, 100}, 2, inside_check);

    vec<vec2> visits{};
    tree.run(vec2{ 30, 30}, [&](const vec2& element){ visits.push_back(element); });
    ASSERT_EQ(1, visits.size());
    ASSERT_EQ(points[0], visits[0]);
}

TEST(QtreeTest, LotOfPointsTest) {
    // 16384 points
    vec<vec2> points{};
    for (i32 x = 0; x < 128; x++) {
        for (i32 y = 0; y < 128; y++) {
            points.emplace_back(x, y);
        }
    }

    const auto inside_check = [](const vec2& point, const vec2& start, const vec2& end){
        return point.x >= start.x && point.x < end.x && point.y >= start.y && point.y < end.y;
    };
    const qtree<vec2> tree = qtree<vec2>::build(std::move(points), vec2{128, 128}, 4, inside_check);

    // Only 256 visits
    vec<vec2> visits{};
    tree.run(vec2{ 30, 30}, [&](const vec2& element){ visits.push_back(element); });
    ASSERT_EQ(256, visits.size());
}

