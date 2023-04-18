//
// Created by Vadim Gush on 12.04.2023.
//

#ifndef UTILITIES_GEOMETRY_H
#define UTILITIES_GEOMETRY_H

#include <glm/glm.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/gtx/intersect.hpp>
#include "collections.h"
#include "types.h"

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using quat = glm::quat;

namespace geometry {

    template <typename P>
    struct line {
        P p1{};
        P p2{};
    };

    template <typename P>
    struct triangle {
        P p1{};
        P p2{};
        P p3{};
    };

    using triangle3 = triangle<vec3>;
    using line3 = line<vec3>;
    using line2 = line<vec2>;

    struct plane {
        vec3 position{};
        vec3 normal{};
    };

    static plane triangle_to_plane(const triangle<vec3> &tr) {
        return {.position = tr.p1, .normal = glm::normalize(glm::cross(tr.p2 - tr.p1, tr.p3 - tr.p1))};
    }

    static opt<line<vec3>> intersection(const plane &plane, const triangle<vec3> &tri) {
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
            return { line<vec3>{*p1, *p2} };
        }
        return {};
    }
}

#endif //UTILITIES_GEOMETRY_H
