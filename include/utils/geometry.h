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

        /**
         * Checks if a point O projected onto the line falls between P1 and P2.
         *
         * @param o point O to project onto the line
         * @return true if point is between P1 and P2 on the line, otherwise false
         */
        bool projection_within(const P& o) const {
            const P dir_l = p2 - p1;
            const f32 t = (glm::dot(o - p1, dir_l) / glm::length(dir_l)) / glm::length(dir_l);
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

        /**
         * Construct a plane from the triangle
         *
         * @param tr triangle
         * @return plane
         */
        static plane from(const triangle<vec3>& tr) {
            return {.position = tr.p1, .normal = glm::normalize(glm::cross(tr.p2 - tr.p1, tr.p3 - tr.p1))};
        }
    };

    struct rectangle {
        vec2 bl{};
        vec2 tr{};

        rectangle() =default;

        rectangle(const vec2& a, const vec2& b):
            bl{ glm::min(a.x, b.x), glm::min(a.y, b.y) },
            tr{ glm::max(a.x, b.x), glm::max(a.y, b.y) } {}

        vec2 size() const { return tr - bl; }

        bool inside_of(const rectangle& other) const {
            return bl.x >= other.bl.x && bl.y >= other.bl.y && tr.x <= other.tr.x && tr.y <= other.tr.y;
        }
    };

    /**
     * Checks if 2 rectangles overlap each other
     *
     * @param rect1 first rectangle
     * @param rect2 second rectangle
     * @return true if overlap
     */
    static bool overlap(const rectangle& rect1, const rectangle& rect2) {
        if (rect1.bl.y >= rect2.tr.y) return false;
        if (rect1.tr.y <= rect2.bl.y) return false;
        if (rect1.tr.x <= rect2.bl.x) return false;
        if (rect1.bl.x >= rect2.tr.x) return false;
        return true;
    }

    static bool same_sign(const f32 a, const f32 b) {
        return a * b >= 0.f;
    }

    static bool eq_zero(const f32 a, const f32 EPSILON) {
        return a > -EPSILON && a < EPSILON;
    }

    /**
     * Calculates intersection between 2 lines (infinitely long)
     *
     * @param l1 first line
     * @param l2 second line
     * @return point of intersection or nothing if there is no intersection
     */
    static opt<vec2> intersection(const line<vec2>& l1, const line<vec2>& l2) {
        constexpr f32 EPSILON = 0.001f;
        const vec2& p1 = l1.p1, p2 = l1.p2, p3 = l2.p1, p4 = l2.p2;

        // First
        const f32 a1 = p2.y - p1.y;
        const f32 b1 = p1.x - p2.x;
        const f32 c1 = p2.x * p1.y - p1.x * p2.y;
        const f32 r3 = a1 * p3.x + b1 * p3.y + c1;
        const f32 r4 = a1 * p4.x + b1 * p4.y + c1;
        if  (!eq_zero(r3, EPSILON) && !eq_zero(r4, EPSILON) && same_sign(r3, r4)) return {};

        // Second
        const f32 a2 = p4.y - p3.y;
        const f32 b2 = p3.x - p4.x;
        const f32 c2 = p4.x * p3.y - p3.x * p4.y;
        const f32 r1 = a2 * p1.x + b2 * p1.y + c2;
        const f32 r2 = a2 * p2.x + b2 * p2.y + c2;
        if  (!eq_zero(r1, EPSILON) && !eq_zero(r2, EPSILON) && same_sign(r1, r2)) return {};

        const f32 d = (p1.x-p2.x)*(p3.y-p4.y)-(p1.y-p2.y)*(p3.x-p4.x);
        if (eq_zero(d, EPSILON)) return {};

        const f32 m1 = p1.x*p2.y-p1.y*p2.x;
        const f32 m2 = p3.x*p4.y-p3.y*p4.x;
        return vec2{
                (m1*(p3.x-p4.x)-(p1.x-p2.x)*m2)/d,
                (m1*(p3.y-p4.y)-(p1.y-p2.y)*m2)/d};
    }

    /**
     * Calculates intersection between plane and triangle.
     * WARNING: if one of the sides of the triangle is on the plane, this function will NOT return anything.
     *
     * @param plane plane
     * @param tri triangle
     * @return line of intersection or nothing if there is no intersection
     */
    static opt<line<vec3>> intersection(const plane &plane, const triangle<vec3> &tri) {
        const vec3 p1p2 = tri.p2 - tri.p1;
        const vec3 p1p3 = tri.p3 - tri.p1;
        const vec3 p2p3 = tri.p3 - tri.p2;

        const vec3 p1p2_n = glm::normalize(p1p2);
        const vec3 p1p3_n = glm::normalize(p1p3);
        const vec3 p2p3_n = glm::normalize(p2p3);

        opt<vec3> p1{};

        f32 int_p1p2_distance = 0;
        const bool int_p1p2 = glm::intersectRayPlane(tri.p1, p1p2_n, plane.position, plane.normal, int_p1p2_distance);
        if (int_p1p2 && int_p1p2_distance < glm::length(p1p2) && int_p1p2_distance > 0) {
            p1.emplace(tri.p1 + (p1p2_n * int_p1p2_distance));
        }

        f32 int_p1p3_distance = 0;
        const bool int_p1p3 = glm::intersectRayPlane(tri.p1, p1p3_n, plane.position, plane.normal, int_p1p3_distance);
        if (int_p1p3 && int_p1p3_distance < glm::length(p1p3) && int_p1p3_distance > 0) {
            const vec3 intersection = tri.p1 + (p1p3_n * int_p1p3_distance);
            if (p1) { return geometry::line3{ *p1, intersection }; }
            p1.emplace(intersection);
        }

        f32 int_p2p3_distance = 0;
        const bool int_p2p3 = glm::intersectRayPlane(tri.p2, p2p3_n, plane.position, plane.normal, int_p2p3_distance);
        if (int_p2p3 && int_p2p3_distance < glm::length(p2p3) && int_p2p3_distance > 0) {
            const vec3 intersection = tri.p2 + (p2p3_n * int_p2p3_distance);
            if (p1) { return geometry::line3{ *p1, intersection }; }
        }
        return {};
    }
}

#endif //UTILITIES_GEOMETRY_H
