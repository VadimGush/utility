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

        bool projection_within(const P& o) const {
            const P dir_o = o - p1;
            const P dir_l = p2 - p1;
            const f32 t = glm::dot(dir_o, dir_l) / glm::length(dir_l);
            return t > 0 && t <= 1.f;
        }
    };

    template <typename P>
    struct triangle {
        P p1{};
        P p2{};
        P p3{};
    };

    template <typename P>
    struct ray {
        P origin{};
        P direction{};
    };

    using triangle3 = triangle<vec3>;
    using line3 = line<vec3>;
    using line2 = line<vec2>;

    struct plane {
        vec3 position{};
        vec3 normal{};
    };

    struct rectangle {
        vec2 a{};
        vec2 b{};

        vec2 bottom_left() const { return { glm::min(a.x, b.x), glm::min(a.y, b.y) }; }
        vec2 top_right() const { return { glm::max(a.x, b.x), glm::max(a.y, b.y) }; }
    };

    static plane triangle_to_plane(const triangle<vec3> &tr) {
        return {.position = tr.p1, .normal = glm::normalize(glm::cross(tr.p2 - tr.p1, tr.p3 - tr.p1))};
    }

    static bool overlap(const rectangle& rect1, const rectangle& rect2) {
        const vec2 rect1_bl = rect1.bottom_left(), rect1_tr = rect1.top_right();
        const vec2 rect2_bl = rect2.bottom_left(), rect2_tr = rect2.top_right();

        if (rect1_bl.y >= rect2_tr.y) return false;
        if (rect1_tr.y <= rect2_bl.y) return false;
        if (rect1_tr.x <= rect2_bl.x) return false;
        if (rect1_bl.x >= rect2_tr.x) return false;

        return true;
    }

    static opt<vec2> intersection(const line<vec2>& l1, const line<vec2>& l2) {
        constexpr f32 EPSILON = 0.001f;
        const vec2& p1 = l1.p1, p2 = l1.p2, p3 = l2.p1, p4 = l2.p2;

        const f32 d = (p1.x-p2.x)*(p3.y-p4.y)-(p1.y-p2.y)*(p3.x-p4.x);
        if (d < EPSILON) return {};

        return vec2{
                ((p1.x*p2.y-p1.y*p2.x)*(p3.x-p4.x)-(p1.x-p2.x)*(p3.x*p4.y-p3.y*p4.x))/d,
                ((p1.x*p2.y-p1.y*p2.x)*(p3.y-p4.y)-(p1.y-p2.y)*(p3.x*p4.y-p3.y*p4.x))/d};
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
