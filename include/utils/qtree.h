//
// Created by Vadim Gush on 17.04.2023.
//

#ifndef UTILITIES_QUAD_TREE_H
#define UTILITIES_QUAD_TREE_H

#include "types.h"
#include "collections.h"
#include "geometry.h"

#include <iostream>

/**
 * Quad-tree. This implementation is not very efficient because it uses quite a lot
 * of memory allocations and all data is randomly distributes across the memory. Which is not cache-friendly.
 *
 * @tparam T type of stored elements
 */
template <typename T>
struct qtree {
private:

    // Tells if the provided element should be placed inside the node
    using distributor = std::function<bool(const T& t, const vec2& bottom_left, const vec2& top_right)>;
    // Called when a particular element is visited
    using visitor = std::function<void(const T& element)>;
    // Selects which node should be visited
    using selector = std::function<bool(const geometry::rectangle& box)>;

    struct node {
        geometry::rectangle box{};
        vec<T> elements{};
        arr<uptr<node>, 4> nodes{};
    };

    node root_{};

public:

    qtree() =default;

    qtree(node&& root): root_(std::move(root)) {}

    /**
     * Select nodes in quad tree and calls visitor per every element in this node
     *
     * @param selector function which tells which nodes should be selected
     * @param visitor called per every element in the selected node
     */
    void select(const selector& selector, const visitor& visitor) const {
        select_(root_, selector, visitor);
    }

    /**
     * Every node which is inside of defined region will be visited.
     *
     * @param region_bl bottom-left corner of the region
     * @param region_tr top-right corner of the region
     * @param visitor called per every element in the selected node
     */
    void region(const vec2& region_bl, const vec2& region_tr, const visitor& visitor) const {
        const geometry::rectangle region_box{ region_bl, region_tr };
        select_(root_, [&](const geometry::rectangle& node_box) {
            return geometry::overlap(region_box, node_box);
        }, visitor);
    }

    /**
     * Every node which is inside of the given radius will be visited.
     *
     * @param center center of the region
     * @param radius radius
     * @param visitor called per every element in the selected node
     */
    void radius(const vec2& center, const f32 radius, visitor& visitor) const {
        region(vec2{ center.x - radius, center.y - radius },
               vec2{ center.x + radius, center.y + radius }, visitor);
    }

    /**
     * Builds quad-tree
     *
     * @param values values which will be stored inside of the quad-tree
     * @param position position of quad-tree in space
     * @param size size of quad-tree in space
     * @param depth depth of the tree
     * @param distributor function to tell which element should be place in which node
     * @return quad-tree ready to use
     */
    static qtree build(vec<T> values,
                       const vec2 position,
                       const vec2 size,
                       const u32 depth,
                       const distributor& distributor) {
        node root{ .box = geometry::rectangle{ position, position + size }, .elements = std::move(values) };
        build_child_nodes_(root, depth, 1, distributor);
        return qtree(std::move(root));
    }

private:

    static void build_child_nodes_(node& n,
                                  const u32 depth,
                                  const u32 current_depth,
                                  const distributor& distributor) {
        if (depth == current_depth) return;

        // create all children nodes
        const vec2 new_size = n.box.size() / 2.f;
        n.nodes[0] = std::make_unique<node>(
                node{ .box = geometry::rectangle{ n.box.bl,
                                                  n.box.bl + new_size} });
        n.nodes[1] = std::make_unique<node>(
                node{ .box = geometry::rectangle{ n.box.bl + vec2{ new_size.x, 0 },
                                                  n.box.bl + vec2{ new_size.x, 0 } + new_size} });
        n.nodes[2] = std::make_unique<node>(
                node{ .box = geometry::rectangle{ n.box.bl + vec2{ 0, new_size.y },
                                                  n.box.bl + vec2{ 0, new_size.y } + new_size} });
        n.nodes[3] = std::make_unique<node>(
                node{ .box = geometry::rectangle{ n.box.bl + vec2{ new_size.x, new_size.y },
                                                  n.box.bl + vec2{ new_size.x, new_size.y } + new_size} });

        vec<T> left{};
        for (const auto& element : n.elements) {
            bool fit = false;
            for (auto& node_ptr : n.nodes) {
                auto& node = *node_ptr;
                if (distributor(element, node.box.bl, node.box.tr)) {
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
                build_child_nodes_(node, depth, current_depth + 1, distributor);
            }
        }
    }

    void select_(const node& node, const selector& selector, const visitor& visitor) const {
        if (selector(node.box)) {
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
