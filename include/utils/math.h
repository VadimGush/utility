//
// Created by Vadim Gush on 12.04.2023.
//

#ifndef UTILITIES_MATH_H
#define UTILITIES_MATH_H

#include <glm/glm.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/gtx/intersect.hpp>
#include "collections.h"
#include "types.h"

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using quat = glm::quat;

struct line2 {
    vec2 p1{};
    vec2 p2{};
};

struct line3 {
    vec3 p1{};
    vec3 p2{};
};

struct triangle3 {
    vec3 p1{};
    vec3 p2{};
    vec3 p3{};
};

struct plane3 {
    vec3 position{};
    vec3 normal{};
};

static plane3 triangle_to_plane(const triangle3& tr) {
    return { .position = tr.p1, .normal = glm::normalize(glm::cross(tr.p2 - tr.p1, tr.p3 - tr.p1)) };
}

static opt<line3> intersect_triangle_plane(const plane3& plane, const triangle3& tri) {
    constexpr f32 EPSILON = 0.0001f;

    const vec3 p1p2_n = glm::normalize(tri.p2 - tri.p1);
    const vec3 p1p3_n = glm::normalize(tri.p3 - tri.p1);
    const vec3 p2p3_n = glm::normalize(tri.p3 - tri.p2);

    opt<vec3> p1{}, p2{};

    if (glm::abs(glm::dot(p1p2_n, plane.normal)) > EPSILON) {
        f32 p1p2_distance = 0;
        const bool p1p2 = glm::intersectRayPlane(tri.p1, p1p2_n, plane.position, plane.normal, p1p2_distance);
        if (p1p2 && p1p2_distance < glm::length(tri.p2 - tri.p1) && p1p2_distance > 0) {
            const vec3 intersection = tri.p1 + (p1p2_n * p1p2_distance);
            if (!p1.has_value()) { p1 = intersection; } else { p2 = intersection; }
        }
    }
    if (glm::abs(glm::dot(p1p3_n, plane.normal)) > EPSILON) {
        f32 p1p3_distance = 0;
        const bool p1p3 = glm::intersectRayPlane(tri.p1, p1p3_n, plane.position, plane.normal, p1p3_distance);
        if (p1p3 && p1p3_distance < glm::length(tri.p3 - tri.p1) && p1p3_distance > 0) {
            const vec3 intersection = tri.p1 + (p1p3_n * p1p3_distance);
            if (!p1.has_value()) { p1 = intersection; } else { p2 = intersection; }
        }
    }
    if (glm::abs(glm::dot(p2p3_n, plane.normal)) > EPSILON) {
        f32 p2p3_distance = 0;
        const bool p2p3 = glm::intersectRayPlane(tri.p2, p2p3_n, plane.position, plane.normal, p2p3_distance);
        if (p2p3 && p2p3_distance < glm::length(tri.p3 - tri.p2) && p2p3_distance > 0) {
            const vec3 intersection = tri.p2 + (p2p3_n * p2p3_distance);
            if (!p1.has_value()) { p1 = intersection; } else { p2 = intersection; }
        }
    }

    if (p1.has_value() && p2.has_value()) {
        if (glm::distance(*p1, *p2) < EPSILON) return {};
        return { line3{ *p1, *p2 } };
    }
    return {};
}

#endif //UTILITIES_MATH_H
