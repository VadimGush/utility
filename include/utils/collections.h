//
// Created by Vadim Gush on 12.04.2023.
//

#ifndef UTILITIES_COLLECTIONS_H
#define UTILITIES_COLLECTIONS_H

#include <vector>
#include <memory>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <set>
#include <string>
#include <optional>
#include <queue>

template <typename T, typename A = std::allocator<T>>
using vec = std::vector<T, A>;

template <typename T>
using uptr = std::unique_ptr<T>;

template <typename T, typename C = std::less<typename std::vector<T>::value_type>, typename A = std::allocator<T>>
using vec_priority_queue = std::priority_queue<T, std::vector<T, A>, C>;

template <typename T>
using sptr = std::shared_ptr<T>;

template <typename K, typename V, typename H = std::hash<K>>
using umap = std::unordered_map<K, V, H>;

template <typename K, typename V>
using map = std::map<K, V>;

template <typename K, typename V>
using mmap = std::multimap<K, V>;

template <typename K, typename H = std::hash<K>>
using uset = std::unordered_set<K, H>;

template <typename K>
using set = std::set<K>;

template <typename K>
using mset = std::multiset<K>;

using str = std::string;

template <typename T>
using opt = std::optional<T>;

template <typename A, typename B>
using pair = std::pair<A, B>;

template <typename T, int N>
using arr = std::array<T, N>;

template <typename V, typename E>
struct result {
private:

    struct SUCCESS {};
    struct FAILURE {};

    bool success_;
    std::variant<V, E> variant_;

    template <typename T>
    result(SUCCESS s, T&& arg): success_(true), variant_(std::in_place_index<0>, std::forward<T>(arg)) {}

    template <typename T>
    result(FAILURE f, T&& arg): success_(false), variant_(std::in_place_index<1>, std::forward<E>(arg)) {}

public:

    result() =delete;

    explicit operator bool() const {
        return success_;
    }

    template <typename T>
    static result success(T&& arg) {
        return result<V, E>(SUCCESS{}, std::forward<T>(arg));
    }

    template <typename T>
    static result failure(T&& arg) {
        return result<V, E>(FAILURE{}, std::forward<T>(arg));
    }

    V& value() {
        return std::get<0>(variant_);
    }

    const V& value() const {
        return std::get<0>(variant_);
    }

    const E& failure() const {
        return std::get<1>(variant_);
    }

};

#endif //UTILITIES_COLLECTIONS_H
