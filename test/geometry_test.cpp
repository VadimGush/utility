//
// Created by Vadim Gush on 18.04.2023.
//
#include <gtest/gtest.h>
#include "../include/utils/geometry.h"

TEST(MathTest, construct_plane_from_triangle) {
    const auto plane = geometry::plane::from(geometry::triangle3 {
        .p1 = vec3{ 1, 1, 1 },
        .p2 = vec3{ 2, 1, 1 },
        .p3 = vec3{ 1, 2, 1 } });

    const vec3 position{1, 1, 1};
    const vec3 normal{0,0,1};
    ASSERT_EQ(position, plane.position);
    ASSERT_EQ(normal, plane.normal);
}

TEST(MathTest, triangle_intersection_on_edge_with_plane) {
    const auto plane = geometry::plane::from(geometry::triangle3{
            .p1 = vec3{ 1, 1, 1 },
            .p2 = vec3{ 2, 1, 1},
            .p3 = vec3{ 1, 2, 1} });

    const geometry::triangle3 triangle{
            .p1 = vec3{ 0, 0, 0 },
            .p2 = vec3{ 0, 0, 1 },
            .p3 = vec3{ 1, 0, 1 } };

    const auto intersection_line = geometry::intersection(plane, triangle);
    ASSERT_FALSE(intersection_line.has_value());
}

TEST(MathTest, triangle_has_intersection_with_plane) {
    const f32 EPSILON = 0.000001f;

    const auto plane = geometry::plane::from(geometry::triangle3{
            .p1 = vec3{ 1, 1, 1 },
            .p2 = vec3{ 2, 1, 1},
            .p3 = vec3{ 1, 2, 1} });

    const geometry::triangle3 triangle{
        .p1 = vec3{ 0, 0, 0 },
        .p2 = vec3{ 0, 0, 2 },
        .p3 = vec3{ 2, 0, 2 } };

    const auto intersection_line = geometry::intersection(plane, triangle);
    ASSERT_TRUE(intersection_line.has_value());

    const vec3 p1 = intersection_line->p1;
    const vec3 p2 = intersection_line->p2;
    ASSERT_TRUE(glm::distance(p1, vec3{0,0,1}) < EPSILON);
    ASSERT_TRUE(glm::distance(p2, vec3{1,0,1}) < EPSILON);
}

TEST(MathTest, rectangle_do_not_overlap) {
    // A above B
    auto r1 = geometry::overlap(geometry::rectangle{ vec2{100,100}, vec2{200,200} },
                                geometry::rectangle{ vec2{100,200}, vec2{200,300}});
    ASSERT_FALSE(r1);

    // A on the right of B
    auto r2 = geometry::overlap(geometry::rectangle{ vec2{100,100}, vec2{200,200} },
                                geometry::rectangle{ vec2{200,100}, vec2{300,200}});
    ASSERT_FALSE(r2);

    // A below B
    auto r3 = geometry::overlap(geometry::rectangle{ vec2{100,100}, vec2{200,200} },
                                geometry::rectangle{ vec2{100,  0}, vec2{200,100}});
    ASSERT_FALSE(r3);

    // A on the left of B
    auto r4 = geometry::overlap(geometry::rectangle{ vec2{100,100}, vec2{200,200} },
                                geometry::rectangle{ vec2{  0,100}, vec2{100,200}});
    ASSERT_FALSE(r4);

    // A on the right top of B
    auto r5 = geometry::overlap(geometry::rectangle{ vec2{100,100}, vec2{200,200} },
                                geometry::rectangle{ vec2{200,200}, vec2{300,300}});
    ASSERT_FALSE(r5);

    // A on the right bottom of B
    auto r6 = geometry::overlap(geometry::rectangle{ vec2{100,100}, vec2{200,200} },
                                geometry::rectangle{ vec2{200,100}, vec2{300,  0}});
    ASSERT_FALSE(r6);

    // A on the left bottom of B
    auto r7 = geometry::overlap(geometry::rectangle{ vec2{100,100}, vec2{200,200} },
                                geometry::rectangle{ vec2{100,100}, vec2{  0,  0}});
    ASSERT_FALSE(r7);

    // A on the left top of B
    auto r8 = geometry::overlap(geometry::rectangle{ vec2{100,100}, vec2{200,200} },
                                geometry::rectangle{ vec2{100,200}, vec2{  0,300}});
    ASSERT_FALSE(r8);
}

TEST(GeometryTest, rectangles_overlap) {
    // A fully inside B
    auto r1 = geometry::overlap(geometry::rectangle{ vec2{100,100}, vec2{200,200} },
                                geometry::rectangle{ vec2{110,110}, vec2{190,190}});
    ASSERT_TRUE(r1);

    // bottom left inside B
    auto r2 = geometry::overlap(geometry::rectangle{ vec2{100,100}, vec2{200,200} },
                                geometry::rectangle{ vec2{190,190}, vec2{290,290}});
    ASSERT_TRUE(r2);

    // top left inside B
    auto r3 = geometry::overlap(geometry::rectangle{ vec2{100,100}, vec2{200,200} },
                                geometry::rectangle{ vec2{150, 50}, vec2{250,150}});
    ASSERT_TRUE(r3);

    // bottom right inside B
    auto r4 = geometry::overlap(geometry::rectangle{ vec2{100,100}, vec2{200,200} },
                                geometry::rectangle{ vec2{ 50,150}, vec2{150,250}});
    ASSERT_TRUE(r4);

    // top right inside B
    auto r5 = geometry::overlap(geometry::rectangle{ vec2{100,100}, vec2{200,200} },
                                geometry::rectangle{ vec2{ 50, 50}, vec2{150,150}});
    ASSERT_TRUE(r5);
}

