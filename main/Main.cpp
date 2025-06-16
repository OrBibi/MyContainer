#include "../include/MyContainer.hpp"
#include <iostream>
using namespace myns;

int main() {
    MyContainer<int> container;
    container.add(7);
    container.add(15);
    container.add(6);
    container.add(1);
    container.add(2);

    std::cout << "Original container: " << container << "\n";

    std::cout << "Ascending Order: ";
    for (auto x : container.ascending_order()) {
        std::cout << x << " ";
    }
    std::cout << "\n";

    std::cout << "Descending Order: ";
    for (auto x : container.descending_order()) {
        std::cout << x << " ";
    }
    std::cout << "\n";

    std::cout << "SideCross Order: ";
    for (auto x : container.sidecross_order()) {
        std::cout << x << " ";
    }
    std::cout << "\n";

    std::cout << "Reverse Order: ";
    for (auto x = container.reverse_order().begin(); x != container.reverse_order().end(); ++x) {
        std::cout << *x << " ";
    }
    std::cout << "\n";

    std::cout << "Original Order: ";
    for (auto x = container.order().begin(); x != container.order().end(); ++x) {
        std::cout << *x << " ";
    }
    std::cout << "\n";

    std::cout << "MiddleOut Order: ";
    for (auto x = container.middle_out_order().begin(); x != container.middle_out_order().end(); ++x) {
        std::cout << *x << " ";
    }
    std::cout << "\n";

    return 0;
} 
