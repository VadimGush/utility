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

    // Tells if the provided element should be placed inside the node with given position and size
    using distributor = std::function<bool(const T& t, const vec2& bottom_left, const vec2& top_right)>;
    // Called when a particular element is visited
    using visitor = std::function<void(const T& element)>;
    // Tells which node should be visited
    using selector = std::function<bool(const vec2& bottom_left, const vec2& top_right)>;

    struct node {
        vec2 position{};
        vec2 size{};
        vec<T> elements{};
        arr<uptr<node>, 4> nodes{};
    };

    distributor inside_check_{};
    node root_{};

public:

    qtree(node&& root, const distributor& inside_check):
        inside_check_(inside_check),
        root_(std::move(root)) {
    }

    void select(const selector& selector, const visitor& visitor) const {
        select_(root_, selector, visitor);
    }

    void region(const vec2& region_bl, const vec2& region_tr, const visitor& visitor) const {
        select_(root_, [&](const vec2& node_bl, const vec2& node_tr) {
            return geometry::overlap(
                    geometry::rectangle{ region_bl, region_tr },
                    geometry::rectangle{ node_bl, node_tr });
        }, visitor);
    }

    void radius(const vec2& center, const f32 radius, visitor& visitor) const {
        region(vec2{ center.x - radius, center.y - radius },
               vec2{ center.x + radius, center.y + radius }, visitor);
    }

    static qtree build(vec<T> values,
                       const vec2 position,
                       const vec2 size,
                       const u32 depth,
                       const distributor& distributor) {
        node root{ .position = position, .size = size, .elements = std::move(values) };
        build_child_nodes(root, depth, 1, distributor);
        return qtree(std::move(root), distributor);
    }

    static void build_child_nodes(node& n,
                                  const u32 depth,
                                  const u32 current_depth,
                                  const distributor& distributor) {
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
                if (distributor(element, node.position, node.position + node.size)) {
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
                build_child_nodes(node, depth, current_depth + 1, distributor);
            }
        }
    }

private:

    void select_(const node& node, const selector& selector, const visitor& visitor) const {
        if (selector(node.position, node.position + node.size)) {
            for (const auto& element : node.elements) {
                visitor(element);
            }
            for (const auto& children_node : node.nodes) {
                if (children_node != nullptr) {
                    select_(*children_node, selector, visitor);
                }
            }
        }
    }


};

#endif //UTILITIES_QUAD_TREE_H
