//
// Created by Vadim Gush on 26.04.2023.
//

#ifndef UTILITIES_GRAPH_H
#define UTILITIES_GRAPH_H

#include "types.h"
#include "geometry.h"

/**
 * Represents a graph with vertices on 2D space
 */
struct graph {
private:
    umap<u32, vec2> vertices_{};
    mmap<u32, u32> connections_{};

public:

    /**
     * Insert a vertex into the graph
     *
     * @param vertex vertex to insert
     * @return id of the inserted vertex
     */
    u32 insert_vertex(const vec2& vertex) {
        const u32 id = vertices_.size();
        vertices_.emplace(id, vertex);
        return id;
    }

    /**
     * Get vertex by the id
     *
     * @param id id of the vertex
     * @return vertex
     */
    const vec2& get_vertex(const u32 id) const {
        return vertices_.at(id);
    }

    /**
     * Connect 2 vertices together
     *
     * @param a id of the first vertex
     * @param b id of the second vertex
     */
    void connect_vertices(const u32 a, const u32 b) {
        connections_.emplace(a, b);
    }

    /**
     * Will remove all vertices which are not connected to anything
     */
    void remove_disconnected() {
        // list of vertices to remove
        vec<u32> remove{};

        for (const auto& vertex : vertices_) {
            const u32 id = vertex.first;
            const auto begin = connections_.lower_bound(id);
            const auto end = connections_.upper_bound(id);
            if (begin == end) remove.emplace_back(id);
        }

        for (const auto& id : remove) {
            vertices_.erase(id);
        }
    }

    /**
     * Removes all unreachable vertices from the graph.
     *
     * @param reachable id of the "reachable" vertex
     */
    void remove_unreachable(const u32 reachable);

    const mmap<u32, u32>& connections() const {
        return connections_;
    }

    const umap<u32, vec2>& vertices() const {
        return vertices_;
    }

};

#endif //UTILITIES_GRAPH_H
