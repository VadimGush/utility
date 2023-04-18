//
// Created by Vadim Gush on 17.04.2023.
//

#ifndef UTILITIES_QUAD_TREE_H
#define UTILITIES_QUAD_TREE_H

#include "types.h"
#include "collections.h"
#include "geometry.h"

#include <iostream>

template <typename T>
struct qtree {
private:
    using inside_check = std::function<bool(const T& t, const vec2& start, const vec2& end)>;
    using visitor = std::function<void(const T& element)>;

    struct node {
        vec2 position{};
        vec2 size{};
        vec<T> elements{};
        arr<uptr<node>, 4> nodes{};
    };

    inside_check inside_check_{};
    node root_{};

    void run_(const node& node, const T& value, const visitor& visitor) const {
        for (const auto& element : node.elements) {
            visitor(element);
        }

        bool fit = false;
        for (const auto& child_node_ptr : node.nodes) {
            if (child_node_ptr != nullptr) {
                const auto &child_node = *child_node_ptr;
                if (inside_check_(value, child_node.position, child_node.position + child_node.size)) {
                    run_(child_node, value, visitor);
                    fit = true;
                }
            }
        }

        // If our point is not inside our children nodes
        // then we should check it against all of them.
        // helpful for line intersection checks where line can overlay
        // multiple regions in quad tree
        if (!fit) {
            for (const auto& child_node_ptr : node.nodes) {
                if (child_node_ptr != nullptr) {
                    run_(*child_node_ptr, value, visitor);
                }
            }
        }
    }

public:

    qtree(node&& root, const inside_check& inside_check):
        inside_check_(inside_check),
        root_(std::move(root)) { }

    void run(const T& value, const std::function<void(const T& element)>& visitor) const {
        if (inside_check_(value, root_.position, root_.position + root_.size)) {
            run_(root_, value, visitor);
        }
    }

    static qtree build(vec<T> values,
                       const vec2 size,
                       const u32 depth,
                       const inside_check& inside) {
        node root{ .position = vec2{}, .size = size, .elements = std::move(values) };
        build_child_nodes(root, depth, 1, inside);
        return qtree(std::move(root), inside);
    }

    static void build_child_nodes(node& n,
                                  const u32 depth,
                                  const u32 current_depth,
                                  const inside_check& inside) {
        if (depth == current_depth) return;

        // create all children nodes
        const vec2 new_size = n.size / 2.f;
        n.nodes[0] = std::make_unique<node>(
                node{ .position = n.position, .size = new_size });
        n.nodes[1] = std::make_unique<node>(
                node{ .position = n.position + vec2{ new_size.x, 0 }, .size = new_size });
        n.nodes[2] = std::make_unique<node>(
                node{ .position = n.position + vec2{ 0, new_size.y }, .size = new_size });
        n.nodes[3] = std::make_unique<node>(
                node{ .position = n.position + vec2{ new_size.x, new_size.y }, .size = new_size });

        vec<T> left{};
        for (const auto& element : n.elements) {
            bool fit = false;
            for (auto& node_ptr : n.nodes) {
                auto& node = *node_ptr;
                if (inside(element, node.position, node.position + node.size)) {
                    fit = true;
                    node.elements.emplace_back(element);
                    break;
                }
            }
            if (!fit) {
                left.emplace_back(element);
            }
        }
        n.elements = std::move(left);

        for (auto& node_ptr : n.nodes) {
            auto& node = *node_ptr;
            if (node.elements.empty()) {
                node_ptr = nullptr;
            } else {
                build_child_nodes(node, depth, current_depth + 1, inside);
            }
        }
    }

};

#endif //UTILITIES_QUAD_TREE_H
