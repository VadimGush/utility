//
// Created by Vadim Gush on 17.04.2023.
//
#include <gtest/gtest.h>
#include <algorithm>
#include "../include/utils/qtree.h"

TEST(QtreeTest, qtree_with_lot_of_points) {
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
    const auto tree = qtree<vec2>::build(std::move(points), vec2{}, vec2{128, 128}, 4, 0, inside_check);

    // Only 256 visits
    u32 elements = 0;
    tree.region(vec2{0,0}, vec2{16,16}, [&](const vec2* ptr, const u32 size){ elements += size; });
    ASSERT_EQ(256, elements);
}

TEST(QtreeTest, qtree_negative_position) {
    // 16384 points
    vec<vec2> points{};
    for (i32 x = -64; x < 64; x++) {
        for (i32 y = -64; y < 64; y++) {
            points.emplace_back(x, y);
        }
    }

    const auto inside_check = [](const vec2& point, const vec2& start, const vec2& end){
        return point.x >= start.x && point.x < end.x && point.y >= start.y && point.y < end.y;
    };
    const auto tree = qtree<vec2>::build(std::move(points), vec2{-64, -64}, vec2{128, 128}, 4, 0, inside_check);

    // Only 256 visits
    u32 elements = 0;
    tree.region(vec2{64,64}, vec2{64-16, 64-16}, [&](const vec2* ptr, const u32 size){ elements += size; });
    ASSERT_EQ(256, elements);
}

TEST(QTreeTest, line_outside_qtree_doesnt_visit_anyting) {
    // 16384 lines
    vec<geometry::line2> lines{};
    for (i32 x = 0; x < 128; x++) {
        for (i32 y = 0; y < 128; y++) {
            lines.emplace_back(geometry::line2{ .p1 = vec2{x, y}, .p2 = vec2{x, y}});
        }
    }

    const auto inside_check = [](const auto& line, const vec2& start, const vec2& end){
        const bool first_point_inside = line.p1.x >= start.x && line.p1.x < end.x && line.p1.y >= start.y && line.p1.y < end.y;
        const bool second_point_inside = line.p2.x >= start.x && line.p2.x < end.x && line.p2.y >= start.y && line.p2.y < end.y;
        return first_point_inside && second_point_inside;
    };
    const auto tree = qtree<geometry::line2>::build(std::move(lines), vec2{}, vec2{128, 128}, 4, 0, inside_check);

    u32 elements = 0;
    tree.region(vec2{128, 128}, vec2{200,200}, [&](const auto* ptr, const u32 size){ elements += size; });
    ASSERT_EQ(0, elements);
}

TEST(QtreeTest, qtree_iterators) {
    vec<vec2> points{};
    for (i32 x = -10; x < 10; x++) {
        for (i32 y = -10; y < 10; y++) {
            points.emplace_back(x, y);
        }
    }

    const auto inside_check = [](const vec2& point, const vec2& start, const vec2& end){
        return point.x >= start.x && point.x < end.x && point.y >= start.y && point.y < end.y; };
    const auto tree = qtree<vec2>::build(points, vec2{}, vec2{128, 128}, 4, 0, inside_check);

    ASSERT_EQ(points.size(), tree.size());
    for (const auto& point : tree) {
        const auto iterator = std::find(points.begin(), points.end(), point);
        ASSERT_TRUE(iterator != points.end());
    }

    for (u32 i = 0; i < tree.size(); i++) {
        const auto iterator = std::find(points.begin(), points.end(), tree[i]);
        ASSERT_TRUE(iterator != points.end());
    }
}

