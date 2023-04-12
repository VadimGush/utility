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
#include <optional>

template <typename T>
using vec = std::vector<T>;

template <typename T>
using uptr = std::unique_ptr<T>;

template <typename T>
using sptr = std::shared_ptr<T>;

template <typename K, typename V>
using umap = std::unordered_map<K, V>;

template <typename K, typename V>
using map = std::map<K, V>;

template <typename K, typename V>
using uset = std::unordered_set<K, V>;

template <typename K, typename V>
using set = std::set<K, V>;

using str = std::string;

template <typename T>
using opt = std::optional<T>;

#endif //UTILITIES_COLLECTIONS_H
