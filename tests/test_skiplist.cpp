#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "project/SkipList.hpp"
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>

TEST_CASE("New skip list is empty") {
    SkipList<int, std::string> list;
    REQUIRE(list.empty());
    REQUIRE(list.size() == 0);
    REQUIRE(list.items().empty());
}

TEST_CASE("Insert and find") {
    SkipList<int, std::string> list(8, 0.5, 42);
    REQUIRE(list.insert(20, "twenty"));
    REQUIRE(list.insert(10, "ten"));
    REQUIRE(list.insert(30, "thirty"));
    REQUIRE(list.size() == 3);
    REQUIRE(list.contains(10));
    REQUIRE(list.contains(20));
    REQUIRE(list.contains(30));
    REQUIRE_FALSE(list.contains(99));
    REQUIRE(*list.find(20) == "twenty");
}

TEST_CASE("Duplicate key updates value without increasing size") {
    SkipList<int, std::string> list;
    REQUIRE(list.insert(7, "old"));
    REQUIRE_FALSE(list.insert(7, "new"));
    REQUIRE(list.size() == 1);
    REQUIRE(*list.find(7) == "new");
}

TEST_CASE("Items are returned in sorted order") {
    SkipList<int, std::string> list;
    list.insert(50, "fifty"); list.insert(10, "ten"); list.insert(40, "forty");
    list.insert(20, "twenty"); list.insert(30, "thirty");
    const auto items = list.items();
    REQUIRE(items.size() == 5);
    REQUIRE(items[0].first == 10);
    REQUIRE(items[1].first == 20);
    REQUIRE(items[2].first == 30);
    REQUIRE(items[3].first == 40);
    REQUIRE(items[4].first == 50);
}

TEST_CASE("Erase removes keys and preserves remaining keys") {
    SkipList<int, std::string> list;
    for (int i = 1; i <= 10; ++i) list.insert(i, std::to_string(i));
    REQUIRE(list.erase(1));
    REQUIRE(list.erase(5));
    REQUIRE(list.erase(10));
    REQUIRE_FALSE(list.erase(100));
    REQUIRE(list.size() == 7);
    REQUIRE_FALSE(list.contains(1));
    REQUIRE_FALSE(list.contains(5));
    REQUIRE_FALSE(list.contains(10));
    REQUIRE(list.contains(2));
    REQUIRE(list.contains(9));
}

TEST_CASE("Const find works") {
    SkipList<int, std::string> mutableList;
    mutableList.insert(12, "twelve");
    const SkipList<int, std::string>& list = mutableList;
    REQUIRE(list.find(12) != nullptr);
    REQUIRE(*list.find(12) == "twelve");
    REQUIRE(list.find(13) == nullptr);
}

TEST_CASE("Deterministic seeds preserve ordered contents") {
    SkipList<int, int> a(12, 0.5, 1234);
    SkipList<int, int> b(12, 0.5, 1234);
    for (int i = 0; i < 100; ++i) { a.insert(i, i*i); b.insert(i, i*i); }
    REQUIRE(a.items() == b.items());
    REQUIRE(a.size() == 100);
    REQUIRE(b.size() == 100);
}

TEST_CASE("Mutable find updates the stored value") {
    SkipList<int, std::string> list;
    list.insert(4, "before");

    auto* value = list.find(4);
    REQUIRE(value != nullptr);
    *value = "after";

    REQUIRE(*list.find(4) == "after");
}

TEST_CASE("Erasing the first and last keys leaves the middle intact") {
    SkipList<int, int> list;
    list.insert(1, 10);
    list.insert(2, 20);
    list.insert(3, 30);

    REQUIRE(list.erase(1));
    REQUIRE(list.erase(3));
    REQUIRE(list.items() == std::vector<std::pair<int, int>>{{2, 20}});
    REQUIRE(list.size() == 1);
    REQUIRE_FALSE(list.erase(1));
    REQUIRE_FALSE(list.erase(3));
}

TEST_CASE("Move construction transfers contents and empties the source") {
    SkipList<std::string, int> original;
    original.insert("beta", 2);
    original.insert("alpha", 1);

    SkipList<std::string, int> moved(std::move(original));

    REQUIRE(original.empty());
    REQUIRE(original.size() == 0);
    REQUIRE(moved.size() == 2);
    REQUIRE(moved.items() ==
            std::vector<std::pair<std::string, int>>{{"alpha", 1}, {"beta", 2}});
}

TEST_CASE("Move assignment replaces the destination contents") {
    SkipList<int, std::string> source;
    source.insert(8, "source");

    SkipList<int, std::string> destination;
    destination.insert(99, "old");
    destination = std::move(source);

    REQUIRE(source.empty());
    REQUIRE(destination.size() == 1);
    REQUIRE(destination.contains(8));
    REQUIRE_FALSE(destination.contains(99));
    REQUIRE(*destination.find(8) == "source");
}

TEST_CASE("Level count stays within the configured bounds") {
    SkipList<int, int> list(4, 0.5, 7);

    REQUIRE(list.levelCount() == 5);
    for (int key = 0; key < 100; ++key)
        list.insert(key, key);

    REQUIRE(list.levelCount() == 5);
}

TEST_CASE("Constructor rejects invalid levels and probabilities") {
    REQUIRE_THROWS_AS((SkipList<int, int>(0)), std::invalid_argument);
    for (double probability : {-0.5, 0.0, 1.0, 1.5,
                               std::numeric_limits<double>::quiet_NaN()}) {
        REQUIRE_THROWS_AS((SkipList<int, int>(4, probability)),
                          std::invalid_argument);
    }
}

TEST_CASE("Empty and single-element lists support lookup and deletion") {
    SkipList<int, int> list(1);
    REQUIRE(list.levelCount() == 2);
    REQUIRE(list.find(0) == nullptr);
    REQUIRE_FALSE(list.contains(0));
    REQUIRE_FALSE(list.erase(0));

    REQUIRE(list.insert(-1, 42));
    REQUIRE(list.find(-2) == nullptr);
    REQUIRE(list.find(0) == nullptr);
    REQUIRE(list.erase(-1));
    REQUIRE(list.empty());
    REQUIRE(list.items().empty());
    REQUIRE_FALSE(list.erase(-1));
    REQUIRE(list.insert(2, 20));
    REQUIRE(*list.find(2) == 20);
}

TEST_CASE("Reverse insertion and deletion work with different seeds and levels") {
    for (std::size_t maxLevel : {1u, 4u, 16u}) {
        for (unsigned int seed : {1u, 42u, 1234u}) {
            for (double probability : {0.25, 0.5, 0.75}) {
                SkipList<int, int> list(maxLevel, probability, seed);
                for (int key = 199; key >= 0; --key) {
                    REQUIRE(list.insert(key, key * 2));
                }
                const auto entries = list.items();
                REQUIRE(entries.size() == 200);
                for (int key = 0; key < 200; ++key) {
                    REQUIRE(entries[key] == std::make_pair(key, key * 2));
                    REQUIRE(list.contains(key));
                    REQUIRE(list.erase(key));
                    REQUIRE_FALSE(list.contains(key));
                }
                REQUIRE(list.empty());
                REQUIRE(list.levelCount() == maxLevel + 1);
            }
        }
    }
}

TEST_CASE("Large lists preserve remaining keys after alternating deletions") {
    SkipList<int, int> list;
    for (int key = 2999; key >= 0; --key) {
        list.insert(key, key + 1);
    }
    for (int key = 0; key < 3000; key += 2) {
        REQUIRE(list.erase(key));
    }
    REQUIRE(list.size() == 1500);
    const SkipList<int, int>& view = list;
    for (int key = 0; key < 3000; ++key) {
        if (key % 2 == 0) {
            REQUIRE(view.find(key) == nullptr);
        } else {
            REQUIRE(view.find(key) != nullptr);
            REQUIRE(*view.find(key) == key + 1);
        }
    }
}

TEST_CASE("Moved-from lists can be queried and reused") {
    SkipList<int, int> source;
    source.insert(1, 10);
    SkipList<int, int> destination(std::move(source));
    REQUIRE(source.find(1) == nullptr);
    REQUIRE_FALSE(source.erase(1));
    REQUIRE(source.items().empty());
    REQUIRE(source.insert(2, 20));
    REQUIRE(*source.find(2) == 20);
    REQUIRE(*destination.find(1) == 10);

    destination = std::move(source);
    REQUIRE_FALSE(destination.contains(1));
    REQUIRE(*destination.find(2) == 20);
    REQUIRE(source.empty());
    REQUIRE(source.insert(3, 30));

    auto& sameList = destination;
    destination = std::move(sameList);
    REQUIRE(destination.size() == 1);
    REQUIRE(*destination.find(2) == 20);
}

TEST_CASE("Erase, destruction, and move assignment release owned values") {
    std::weak_ptr<int> erasedValue;
    std::weak_ptr<int> destroyedValue;
    std::weak_ptr<int> replacedValue;
    {
        SkipList<int, std::shared_ptr<int>> source;
        auto value = std::make_shared<int>(10);
        erasedValue = value;
        source.insert(1, value);
        value.reset();
        REQUIRE_FALSE(erasedValue.expired());
        REQUIRE(source.erase(1));
        REQUIRE(erasedValue.expired());

        value = std::make_shared<int>(20);
        destroyedValue = value;
        source.insert(2, value);
        value.reset();

        SkipList<int, std::shared_ptr<int>> destination;
        value = std::make_shared<int>(30);
        replacedValue = value;
        destination.insert(3, value);
        value.reset();

        destination = std::move(source);
        REQUIRE(replacedValue.expired());
        REQUIRE_FALSE(destroyedValue.expired());
    }
    REQUIRE(destroyedValue.expired());
}
