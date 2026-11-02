#include "project/SkipList.hpp"
#include <iostream>
#include <string>

int main() {
    SkipList<int, std::string> index(8, 0.5, 42);
    index.insert(40, "Forty");
    index.insert(10, "Ten");
    index.insert(30, "Thirty");
    index.insert(20, "Twenty");
    index.insert(50, "Fifty");

    std::cout << "Skip list contents:
";
    for (const auto& [key, value] : index.items())
        std::cout << key << " -> " << value << '\n';

    std::cout << "Contains 30: " << std::boolalpha << index.contains(30) << '\n';
    std::cout << "Contains 99: " << index.contains(99) << '\n';
    index.erase(30);
    std::cout << "After erasing 30, size = " << index.size() << '\n';
}
