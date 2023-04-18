//
// Created by Vadim Gush on 17.04.2023.
//
#include <gtest/gtest.h>
#include "../include/utils/qtree.h"

TEST(QtreeTest, qtree_with_couple_of_points) {
    vec<vec2> points{
        vec2{25, 25},
        vec2{75, 75},
        vec2{25, 75},
        vec2{75, 25},
    };
    const auto inside_check = [](const vec2& point, const vec2& start, const vec2& end){
        return point.x >= start.x && point.x < end.x && point.y >= start.y && point.y < end.y;
    };
    const auto tree = qtree<vec2>::build(points, vec2{100, 100}, 2, inside_check);

    vec<vec2> visits{};
    tree.run(vec2{ 30, 30}, [&](const vec2& element){ visits.push_back(element); });
    ASSERT_EQ(1, visits.size());
    ASSERT_EQ(points[0], visits[0]);
}

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
    const auto tree = qtree<vec2>::build(std::move(points), vec2{128, 128}, 4, inside_check);

    // Only 256 visits
    i32 visits = 0;
    tree.run(vec2{ 30, 30}, [&](const vec2& element){ visits += 1; });
    ASSERT_EQ(256, visits);
}

TEST(QTreeTest, qtree_correct_behaviour_with_line_intersection) {
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
    const auto tree = qtree<geometry::line2>::build(std::move(lines), vec2{128, 128}, 4, inside_check);

    // intersect only with 4096
    i32 visits = 0;
    tree.run(geometry::line2{ .p1 = vec2{65, 65}, .p2 = vec2{127, 127} }, [&](const auto& element){ visits += 1; });
    ASSERT_EQ(4096, visits);
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
    const auto tree = qtree<geometry::line2>::build(std::move(lines), vec2{128, 128}, 4, inside_check);

    // intersect with 0, because line outside our quad tree
    i32 visits = 0;
    tree.run(geometry::line2{ .p1 = vec2{200, 200}, .p2 = vec2{210, 210} }, [&](const auto& element){ visits += 1; });
    ASSERT_EQ(0, visits);
}

