#pragma once

#include <stdexcept>

// Lab 21: Skip Lists; Probabilistic Search Structures

template <typename Key, typename Value>
SkipList<Key, Value>::SkipList(std::size_t maxLevel, double probability, unsigned int seed)
    : head_(nullptr), size_(0), maxLevel_(maxLevel), probability_(probability),
      generator_(seed), distribution_(0.0, 1.0) {
    if (maxLevel_ == 0) {
        throw std::invalid_argument("maxLevel must be greater than zero");
    }
    if (!(probability_ > 0.0 && probability_ < 1.0)) {
        throw std::invalid_argument("probability must be between zero and one");
    }
    head_ = new Node(Key{}, Value{}, maxLevel_);
}

template <typename Key, typename Value>
SkipList<Key, Value>::~SkipList() { clear(); }

template <typename Key, typename Value>
SkipList<Key, Value>::SkipList(SkipList&& other) noexcept
    : head_(other.head_), size_(other.size_), maxLevel_(other.maxLevel_),
      probability_(other.probability_), generator_(std::move(other.generator_)),
      distribution_(other.distribution_) {
    other.head_ = nullptr;
    other.size_ = 0;
}

template <typename Key, typename Value>
SkipList<Key, Value>& SkipList<Key, Value>::operator=(SkipList&& other) noexcept {
    if (this != &other) {
        clear();
        head_ = other.head_;
        size_ = other.size_;
        maxLevel_ = other.maxLevel_;
        probability_ = other.probability_;
        generator_ = std::move(other.generator_);
        distribution_ = other.distribution_;
        other.head_ = nullptr;
        other.size_ = 0;
    }
    return *this;
}

template <typename Key, typename Value>
std::size_t SkipList<Key, Value>::randomLevel() {
    std::size_t level = 0;
    while (level < maxLevel_ && distribution_(generator_) < probability_) {
        ++level;
    }
    return level;
}

template <typename Key, typename Value>
void SkipList<Key, Value>::clear() {
    if (head_) {
        Node* current = head_->forward[0];
        while (current) {
            Node* next = current->forward[0];
            delete current;
            current = next;
        }
        delete head_;
        head_ = nullptr;
    }
    size_ = 0;
}

template <typename Key, typename Value>
bool SkipList<Key, Value>::insert(const Key& key, const Value& value) {
    // A moved-from list can be used again by recreating its sentinel.
    if (!head_) {
        head_ = new Node(Key{}, Value{}, maxLevel_);
    }

    std::vector<Node*> predecessors(maxLevel_ + 1, head_);
    Node* current = head_;
    for (std::size_t level = maxLevel_ + 1; level > 0; --level) {
        const std::size_t index = level - 1;
        while (current->forward[index] && current->forward[index]->key < key) {
            current = current->forward[index];
        }
        predecessors[index] = current;
    }

    current = current->forward[0];
    // The candidate is not less than key; check equivalence using only <.
    if (current && !(key < current->key)) {
        current->value = value;
        return false;
    }

    const std::size_t height = randomLevel();
    Node* node = new Node(key, value, height);
    for (std::size_t level = 0; level <= height; ++level) {
        node->forward[level] = predecessors[level]->forward[level];
        predecessors[level]->forward[level] = node;
    }
    ++size_;
    return true;
}

template <typename Key, typename Value>
bool SkipList<Key, Value>::erase(const Key& key) {
    if (!head_) {
        return false;
    }

    std::vector<Node*> predecessors(maxLevel_ + 1, head_);
    Node* current = head_;
    for (std::size_t level = maxLevel_ + 1; level > 0; --level) {
        const std::size_t index = level - 1;
        while (current->forward[index] && current->forward[index]->key < key) {
            current = current->forward[index];
        }
        predecessors[index] = current;
    }

    Node* target = current->forward[0];
    if (!target || key < target->key) {
        return false;
    }

    for (std::size_t level = 0; level < target->forward.size(); ++level) {
        predecessors[level]->forward[level] = target->forward[level];
    }
    delete target;
    --size_;
    return true;
}

template <typename Key, typename Value>
bool SkipList<Key, Value>::contains(const Key& key) const { return find(key) != nullptr; }

template <typename Key, typename Value>
Value* SkipList<Key, Value>::find(const Key& key) {
    if (!head_) {
        return nullptr;
    }
    Node* current = head_;
    for (std::size_t level = maxLevel_ + 1; level > 0; --level) {
        const std::size_t index = level - 1;
        while (current->forward[index] && current->forward[index]->key < key) {
            current = current->forward[index];
        }
    }
    current = current->forward[0];
    return current && !(key < current->key) ? &current->value : nullptr;
}

template <typename Key, typename Value>
const Value* SkipList<Key, Value>::find(const Key& key) const {
    if (!head_) {
        return nullptr;
    }
    const Node* current = head_;
    for (std::size_t level = maxLevel_ + 1; level > 0; --level) {
        const std::size_t index = level - 1;
        while (current->forward[index] && current->forward[index]->key < key) {
            current = current->forward[index];
        }
    }
    current = current->forward[0];
    return current && !(key < current->key) ? &current->value : nullptr;
}

template <typename Key, typename Value>
std::size_t SkipList<Key, Value>::size() const noexcept { return size_; }

template <typename Key, typename Value>
bool SkipList<Key, Value>::empty() const noexcept { return size_ == 0; }

template <typename Key, typename Value>
std::vector<std::pair<Key, Value>> SkipList<Key, Value>::items() const {
    std::vector<std::pair<Key, Value>> entries;
    entries.reserve(size_);
    if (head_) {
        const Node* current = head_->forward[0];
        while (current) {
            entries.emplace_back(current->key, current->value);
            current = current->forward[0];
        }
    }
    return entries;
}

template <typename Key, typename Value>
std::size_t SkipList<Key, Value>::levelCount() const noexcept {
    return head_ ? head_->forward.size() : 0;
}
