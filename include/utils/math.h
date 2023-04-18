//
// Created by Vadim Gush on 12.04.2023.
//

#ifndef UTILITIES_MATH_H
#define UTILITIES_MATH_H

#include <glm/glm.hpp>
#include <glm/ext/quaternion_float.hpp>

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

#endif //UTILITIES_MATH_H
