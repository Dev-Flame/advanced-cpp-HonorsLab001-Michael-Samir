#pragma once

#include <cstddef>
#include <random>
#include <utility>
#include <vector>

template <typename Key, typename Value>
class SkipList {
private:
    struct Node {
        Key key;
        Value value;
        std::vector<Node*> forward;
        Node(const Key& k, const Value& v, std::size_t level)
            : key(k), value(v), forward(level + 1, nullptr) {}
    };

    Node* head_;
    std::size_t size_;
    std::size_t maxLevel_;
    double probability_;
    std::mt19937 generator_;
    std::uniform_real_distribution<double> distribution_;

    std::size_t randomLevel();
    void clear();

public:
    explicit SkipList(std::size_t maxLevel = 16,
                      double probability = 0.5,
                      unsigned int seed = 42);
    ~SkipList();

    SkipList(const SkipList&) = delete;
    SkipList& operator=(const SkipList&) = delete;

    SkipList(SkipList&& other) noexcept;
    SkipList& operator=(SkipList&& other) noexcept;

    bool insert(const Key& key, const Value& value);
    bool erase(const Key& key);
    bool contains(const Key& key) const;
    Value* find(const Key& key);
    const Value* find(const Key& key) const;

    std::size_t size() const noexcept;
    bool empty() const noexcept;
    std::vector<std::pair<Key, Value>> items() const;
    std::size_t levelCount() const noexcept;
};

#include "project/SkipList.tpp"
