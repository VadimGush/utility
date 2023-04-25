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

    // Tells if the provided element should be placed inside the temp_node
    using distributor = std::function<bool(const T& t, const vec2& bottom_left, const vec2& top_right)>;
    // Called when a particular element is visited
    using visitor = std::function<void(const T* ptr, const u32 size)>;
    // Selects which temp_node should be visited
    using selector = std::function<bool(const geometry::rectangle& box)>;

    // Only used for initial construction of quad tree
    struct temp_node {
        geometry::rectangle box{};
        vec<T> elements{};
        arr<uptr<temp_node>, 4> nodes{};
        u32 total_elements = 0;
    };

    struct node {
        geometry::rectangle box{};
        u32 elements_start_id = 0;
        u32 elements_total_size = 0;
        u32 elements_size = 0;
        arr<u32, 4> nodes_id{};
    };

    vec<T> elements_{};
    vec<node> nodes_{};

public:

    qtree() =default;

    qtree(vec<T> elements, vec<node> nodes): elements_(std::move(elements)), nodes_(std::move(nodes)) {}

    /**
     * Select nodes in quad tree and calls visitor per every element in this temp_node
     *
     * @param selector function which tells which nodes should be selected
     * @param visitor called per every element in the selected temp_node
     */
    void select(const selector& selector, const visitor& visitor) const {
        select_(nodes_[0], selector, visitor);
    }

    /**
     * Every temp_node which is inside of defined region will be visited.
     *
     * @param region_bl bottom-left corner of the region
     * @param region_tr top-right corner of the region
     * @param visitor called per every element in the selected temp_node
     */
    void region(const vec2& region_bl, const vec2& region_tr, const visitor& visitor) const {
        const geometry::rectangle region_box{ region_bl, region_tr };
        region_(nodes_[0], region_box, visitor);
    }

    /**
     * Every temp_node which is inside of the given radius will be visited.
     *
     * @param center center of the region
     * @param radius radius
     * @param visitor called per every element in the selected temp_node
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
     * @param distributor function to tell which element should be place in which temp_node
     * @return quad-tree ready to use
     */
    static qtree build(vec<T> values,
                       const vec2 position,
                       const vec2 size,
                       const u32 depth,
                       const distributor& distributor) {
        const u32 total_elements = values.size();
        temp_node root{ .box = geometry::rectangle{position, position + size },
                        .elements = std::move(values),
                        .total_elements = total_elements };
        build_child_nodes_(root, depth, 1, distributor);

        vec<T> elements{};
        elements.reserve(total_elements);
        vec<node> nodes{};
        build_static_qtree_(root, elements, nodes);

        return qtree(std::move(elements), std::move(nodes));
    }

private:

    static void build_static_qtree_(temp_node& n, vec<T>& elements, vec<node>& nodes) {
        const u32 node_id = nodes.size();
        nodes.emplace_back(node{
            .box = n.box,
            .elements_size = static_cast<u32>(n.elements.size()),
            .elements_total_size = n.total_elements,
            .elements_start_id = static_cast<u32>(elements.size()) });

        for (T& element : n.elements) {
            elements.emplace_back(std::move(element));
        }

        for (u32 child_id = 0; child_id < 4; child_id++) {
            const auto& child_node = n.nodes[child_id];
            if (child_node != nullptr) {
                nodes[node_id].nodes_id[child_id] = nodes.size();
                build_static_qtree_(*child_node, elements, nodes);
            }
        }
    }

    static void build_child_nodes_(temp_node& n,
                                   const u32 depth,
                                   const u32 current_depth,
                                   const distributor& distributor) {
        if (depth == current_depth) return;

        // create all children nodes
        const vec2 new_size = n.box.size() / 2.f;
        n.nodes[0] = std::make_unique<temp_node>(
                temp_node{ .box = geometry::rectangle{ n.box.bl,
                                                  n.box.bl + new_size} });
        n.nodes[1] = std::make_unique<temp_node>(
                temp_node{ .box = geometry::rectangle{ n.box.bl + vec2{ new_size.x, 0 },
                                                  n.box.bl + vec2{ new_size.x, 0 } + new_size} });
        n.nodes[2] = std::make_unique<temp_node>(
                temp_node{ .box = geometry::rectangle{ n.box.bl + vec2{ 0, new_size.y },
                                                  n.box.bl + vec2{ 0, new_size.y } + new_size} });
        n.nodes[3] = std::make_unique<temp_node>(
                temp_node{ .box = geometry::rectangle{ n.box.bl + vec2{ new_size.x, new_size.y },
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
                node.total_elements = node.elements.size();
                build_child_nodes_(node, depth, current_depth + 1, distributor);
            }
        }
    }

    void region_(const node& node, const geometry::rectangle& region, const visitor& visitor) const {
        const bool inside = node.box.inside_of(region);
        if (inside) {
            // Is our node is fully inside the provided region we don't want
            // to spend time visiting every child node in tree. Instead, we will just select
            // all elements at once.
            visitor(&elements_[node.elements_start_id], node.elements_total_size);

        } else if (geometry::overlap(node.box, region)) {
            visitor(&elements_[node.elements_start_id], node.elements_size);

            for (const u32 child_id : node.nodes_id) {
                if (child_id != 0) region_(nodes_[child_id], region, visitor);
            }
        }
    }

    void select_(const node& node, const selector& selector, const visitor& visitor) const {
        if (selector(node.box)) {
            visitor(&elements_[node.elements_start_id], node.elements_size);

            for (const u32 child_id : node.nodes_id) {
                if (child_id != 0) select_(nodes_[child_id], selector, visitor);
            }
        }
    }


};

#endif //UTILITIES_QUAD_TREE_H
