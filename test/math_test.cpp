//
// Created by Vadim Gush on 18.04.2023.
//
#include <gtest/gtest.h>
#include "../include/utils/math.h"

TEST(MathTest, construct_plane_fromt_triangle) {
    const auto plane = triangle_to_plane(triangle3{
        .p1 = vec3{ 1, 1, 1 },
        .p2 = vec3{ 2, 1, 1 },
        .p3 = vec3{ 1, 2, 1 } });

    const vec3 position{1, 1, 1};
    const vec3 normal{0,0,1};
    ASSERT_EQ(position, plane.position);
    ASSERT_EQ(normal, plane.normal);
}

TEST(MathTest, triangle_intersection_on_edge_with_plane) {
    const auto plane = triangle_to_plane(triangle3{
            .p1 = vec3{ 1, 1, 1 },
            .p2 = vec3{ 2, 1, 1},
            .p3 = vec3{ 1, 2, 1} });

    const triangle3 triangle{
            .p1 = vec3{ 0, 0, 0 },
            .p2 = vec3{ 0, 0, 1 },
            .p3 = vec3{ 1, 0, 1 } };

    const auto intersection_line = intersect_triangle_plane(plane, triangle);
    ASSERT_FALSE(intersection_line.has_value());
}

TEST(MathTest, triangle_has_intersection_with_plane) {
    const f32 EPSILON = 0.000001f;

    const auto plane = triangle_to_plane(triangle3{
            .p1 = vec3{ 1, 1, 1 },
            .p2 = vec3{ 2, 1, 1},
            .p3 = vec3{ 1, 2, 1} });

    const triangle3 triangle{
        .p1 = vec3{ 0, 0, 0 },
        .p2 = vec3{ 0, 0, 2 },
        .p3 = vec3{ 2, 0, 2 } };

    const auto intersection_line = intersect_triangle_plane(plane, triangle);
    ASSERT_TRUE(intersection_line.has_value());

    const vec3 p1 = intersection_line->p1;
    const vec3 p2 = intersection_line->p2;
    ASSERT_TRUE(glm::distance(p1, vec3{0,0,1}) < EPSILON);
    ASSERT_TRUE(glm::distance(p2, vec3{1,0,1}) < EPSILON);
}

