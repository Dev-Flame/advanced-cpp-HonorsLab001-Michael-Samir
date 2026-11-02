# Lab 21 — Skip Lists; Probabilistic Search Structures

## Overview

Implement a generic **skip list**, a probabilistic ordered search structure that augments a sorted linked list with multiple forward levels. With appropriate randomization, search, insertion, and deletion have expected **O(log n)** time while retaining a simple linked-node representation.

You will use C++ templates, dynamic memory, sentinel nodes, pointer manipulation, move semantics, and randomized level generation.

## Learning Objectives

- Explain how multiple levels accelerate an ordered linked list.
- Implement top-down skip-list search.
- Implement insertion using predecessor arrays.
- Implement deletion while maintaining every level.
- Generate node heights using a geometric probability distribution.
- Analyze expected and worst-case complexity.
- Manage dynamic nodes safely with RAII.
- Build a reusable templated ordered dictionary.

## Required Work

Implement all TODOs in `include/project/SkipList.tpp`. **Do not change the public API in `SkipList.hpp`.**

### Constructor

Validate that `maxLevel > 0` and `0 < probability < 1`, then create the sentinel head node with `maxLevel + 1` forward pointers.

### Insert

`insert(key, value)` must return `true` for a new key. If the key already exists, replace its value, return `false`, and do not change `size()`.

Use a predecessor array to remember the last node visited at each level. Generate a random level for each new node and splice it into all levels from 0 through that level.

### Search

`find` and `contains` must search from the highest level downward. Never scan level 0 from the beginning for every lookup.

### Erase

Remove the target node from every level in which it occurs, delete it exactly once, decrement the size, and return whether removal occurred.

### Random Levels

Start each node at level 0. Repeatedly promote while `distribution_() < probability_`, stopping at `maxLevel_`. The supplied seed makes the random sequence reproducible.

## Complexity

| Operation | Expected | Worst Case |
|---|---:|---:|
| Search | O(log n) | O(n) |
| Insert | O(log n) | O(n) |
| Erase | O(log n) | O(n) |
| Traversal | O(n) | O(n) |

The worst case is linear because a probabilistic structure does not provide the deterministic balancing guarantee of a balanced search tree.

## Memory Requirements

The destructor and `clear()` must free every data node and the sentinel. Move construction and move assignment must transfer ownership safely and leave the moved-from object destructible.

## Restrictions

Do not use `std::map`, `std::set`, `std::unordered_map`, or another container to implement the search structure. The skip list itself must perform the search, insertion, and deletion operations.

## Testing

Run:

```bash
make
make run
make test
```

Also test empty lists, one element, duplicate keys, reverse-order insertion, deletion of first/last/only/missing keys, large inputs, different seeds, and different maximum levels.

## Analysis

Complete `docs/analysis.md` with your explanation of skip-list search, randomization, expected/worst-case complexity, and comparison with a balanced binary search tree.

## Submission

Commit and push your completed implementation. The GitHub Actions workflow will compile the project and run the tests.
