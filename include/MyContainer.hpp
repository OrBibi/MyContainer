#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <numeric>

namespace myns {

template<typename T>
class MyContainer {
private:
    std::vector<T> data;  // Internal storage for elements

    // --- STATIC ASSERTS: enforce required traits for T at compile-time ---

    // Needed for copying elements (e.g., push_back, sorting)
    static_assert(std::is_copy_constructible<T>::value,
                  "T must be copy constructible");

    // Needed to allow assigning T values (e.g., in vector or sorting)
    static_assert(std::is_copy_assignable<T>::value,
                  "T must be copy assignable");

    // Needed to support move operations (e.g., when resizing vector)
    static_assert(std::is_move_constructible<T>::value,
                  "T must be move constructible");

    // Needed to support move assignment (e.g., internal vector operations)
    static_assert(std::is_move_assignable<T>::value,
                  "T must be move assignable");

    // Required for equality-based operations (e.g., remove, find)
    static_assert(std::is_same<decltype(std::declval<T>() == std::declval<T>()), bool>::value,
                  "T must support operator== returning bool");

    // Required for sorting in iterators like AscendingOrder, DescendingOrder
    static_assert(std::is_same<decltype(std::declval<T>() < std::declval<T>()), bool>::value,
                  "T must support operator< returning bool");

public:
    MyContainer();                             // Default constructor
    void add(const T& value);                  // Add element
    void remove(const T& value);               // Remove element(s)
    size_t size() const;                       // Return number of elements
    const std::vector<T>& get_data() const;    // Access underlying vector

    // Access individual elements
    const T& at(size_t index) const;
    T& at(size_t index);
    const T& operator[](size_t index) const;
    T& operator[](size_t index);

    // Iterator classes
    class AscendingOrder;
    class DescendingOrder;
    class SideCrossOrder;
    class ReverseOrder;
    class Order;
    class MiddleOutOrder;

    // Accessors to iterators
    AscendingOrder ascending_order() const;
    DescendingOrder descending_order() const;
    SideCrossOrder sidecross_order() const;
    ReverseOrder reverse_order() const;
    Order order() const;
    MiddleOutOrder middle_out_order() const;
};


// Implementation

// Constructor for the container - initializes an empty container
template<typename T>
MyContainer<T>::MyContainer() {}

// Adds a new element to the container
template<typename T>
void MyContainer<T>::add(const T& value) {
    data.push_back(value);
}

// Removes all occurrences of a given value from the container
// Throws an exception if the element is not found
template<typename T>
void MyContainer<T>::remove(const T& value) {
    // Check if the value exists before attempting to remove it
    if (std::find(data.begin(), data.end(), value) == data.end()) {
        throw std::runtime_error("Element not found");
    }

    // Remove all occurrences of the value
    auto it = std::remove(data.begin(), data.end(), value);
    data.erase(it, data.end());
}

// Returns the number of elements in the container
template<typename T>
size_t MyContainer<T>::size() const {
    return data.size();
}

// Provides read-only access to the underlying vector
template<typename T>
const std::vector<T>& MyContainer<T>::get_data() const {
    return data;
}

// Access element with bounds checking (read-only)
template<typename T>
const T& MyContainer<T>::at(size_t index) const {
    return data.at(index);  // Will throw std::out_of_range if invalid
}

// Access element with bounds checking (read-write)
template<typename T>
T& MyContainer<T>::at(size_t index) {
    return data.at(index);
}

// Access element without bounds checking (read-only)
template<typename T>
const T& MyContainer<T>::operator[](size_t index) const {
    return data[index];  // No bounds check
}

// Access element without bounds checking (read-write)
template<typename T>
T& MyContainer<T>::operator[](size_t index) {
    return data[index];  // No bounds check
}

// Stream output operator for printing the container
template<typename T>
std::ostream& operator<<(std::ostream& os, const MyContainer<T>& c) {
    os << "{";
    for (size_t i = 0; i < c.get_data().size(); ++i) {
        os << c.get_data()[i];
        if (i + 1 < c.get_data().size()) os << ", ";
    }
    os << "}";
    return os;
}

// ========================== ITERATORS IMPLEMENTATION ==========================

//
// AscendingOrder iterator - traverses the container in ascending (sorted) order
//
template<typename T>
class MyContainer<T>::AscendingOrder {
    const MyContainer& cont;     // Reference to the original container
    std::vector<T> sorted;       // Copy of the container's data, sorted in ascending order
    size_t pos = 0;              // Current index in the sorted vector

public:
    AscendingOrder(const MyContainer& c) : cont(c) {
        sorted = c.get_data();                 // Copy data from container
        std::sort(sorted.begin(), sorted.end()); // Sort ascending
    }

    const T& operator*() const {
        if (pos >= sorted.size()) throw std::out_of_range("AscendingOrder dereference out of bounds");
        return sorted[pos];                   // Return element at current position
    }

    AscendingOrder& operator++() { ++pos; return *this; } // Move to next element
    bool operator==(const AscendingOrder& other) const { return pos == other.pos; } // Compare positions
    bool operator!=(const AscendingOrder& other) const { return !(*this == other); } // Negated equality
    AscendingOrder begin() const { return *this; } // Begin at position 0
    AscendingOrder end() const { AscendingOrder it = *this; it.pos = sorted.size(); return it; } // End at size
};

//
// DescendingOrder iterator - traverses the container in descending order
//
template<typename T>
class MyContainer<T>::DescendingOrder {
    const MyContainer& cont;     // Reference to the container
    std::vector<T> sorted;       // Sorted in descending order
    size_t pos = 0;              // Current index

public:
    DescendingOrder(const MyContainer& c) : cont(c) {
        sorted = c.get_data();                               // Copy data
        std::sort(sorted.begin(), sorted.end(), std::greater<T>()); // Sort descending
    }

    const T& operator*() const {
        if (pos >= sorted.size()) throw std::out_of_range("DescendingOrder dereference out of bounds");
        return sorted[pos];
    }

    DescendingOrder& operator++() { ++pos; return *this; }
    bool operator==(const DescendingOrder& other) const { return pos == other.pos; }
    bool operator!=(const DescendingOrder& other) const { return !(*this == other); }
    DescendingOrder begin() const { return *this; }
    DescendingOrder end() const { DescendingOrder it = *this; it.pos = sorted.size(); return it; }
};

//
// SideCrossOrder iterator - alternates between smallest and largest values
//
template<typename T>
class MyContainer<T>::SideCrossOrder {
    const MyContainer& cont;     // Reference to container
    std::vector<T> order;        // Elements arranged by side-cross logic
    size_t pos = 0;

public:
    SideCrossOrder(const MyContainer& c) : cont(c) {
        std::vector<T> sorted = c.get_data();  // Copy data
        std::sort(sorted.begin(), sorted.end()); // Sort ascending
        if (sorted.empty()) return;            // If empty, leave order empty

        size_t left = 0;
        size_t right = sorted.size() - 1;

        // Alternate from both ends: left, right, next left, next right...
        while (left <= right) {
            if (left == right) {
                order.push_back(sorted[left]); // Only one element left
            } else {
                order.push_back(sorted[left]);
                order.push_back(sorted[right]);
            }
            ++left;
            if (right > 0) --right;
        }
    }

    const T& operator*() const {
        if (pos >= order.size()) throw std::out_of_range("SideCrossOrder dereference out of bounds");
        return order[pos];
    }

    SideCrossOrder& operator++() { ++pos; return *this; }
    bool operator==(const SideCrossOrder& other) const { return pos == other.pos; }
    bool operator!=(const SideCrossOrder& other) const { return !(*this == other); }
    SideCrossOrder begin() const { return *this; }
    SideCrossOrder end() const { SideCrossOrder it = *this; it.pos = order.size(); return it; }
};

//
// ReverseOrder iterator - traverses the container in reverse (from last to first)
//
template<typename T>
class MyContainer<T>::ReverseOrder {
    const MyContainer& cont;   // Reference to container
    size_t pos = 0;            // Logical position from end

public:
    ReverseOrder(const MyContainer& c) : cont(c), pos(0) {}

    const T& operator*() const {
        const auto& data = cont.get_data();
        if (pos >= data.size()) throw std::out_of_range("ReverseOrder dereference out of bounds");
        return data[data.size() - 1 - pos];  // Access in reverse
    }

    ReverseOrder& operator++() { ++pos; return *this; }
    bool operator==(const ReverseOrder& other) const { return pos == other.pos; }
    bool operator!=(const ReverseOrder& other) const { return !(*this == other); }
    ReverseOrder begin() const { return *this; }
    ReverseOrder end() const { ReverseOrder it = *this; it.pos = cont.get_data().size(); return it; }
};

//
// Order iterator - returns elements in their original insertion order
//
template<typename T>
class MyContainer<T>::Order {
    const MyContainer& cont;   // Reference to container
    size_t pos = 0;            // Index from beginning

public:
    Order(const MyContainer& c) : cont(c), pos(0) {}

    const T& operator*() const {
        const auto& data = cont.get_data();
        if (pos >= data.size()) throw std::out_of_range("Order dereference out of bounds");
        return data[pos];  // Return element by insertion order
    }

    Order& operator++() { ++pos; return *this; }
    bool operator==(const Order& other) const { return pos == other.pos; }
    bool operator!=(const Order& other) const { return !(*this == other); }
    Order begin() const { return *this; }
    Order end() const { Order it = *this; it.pos = cont.get_data().size(); return it; }
};

//
// MiddleOutOrder iterator - starts from the middle element, then alternates left/right
//
template<typename T>
class MyContainer<T>::MiddleOutOrder {
    const MyContainer& cont;
    std::vector<T> order;      // Elements arranged from middle outward
    size_t pos = 0;

public:
    MiddleOutOrder(const MyContainer& c) : cont(c) {
        const auto& data = c.get_data();
        if (data.empty()) return;

        size_t mid = data.size() / 2;
        order.push_back(data[mid]);  // Start from middle

        int left = static_cast<int>(mid) - 1;
        int right = static_cast<int>(mid) + 1;

        // Alternate outward: left, right, left, right...
        while (left >= 0 || right < static_cast<int>(data.size())) {
            if (left >= 0) order.push_back(data[left--]);
            if (right < static_cast<int>(data.size())) order.push_back(data[right++]);
        }
    }

    const T& operator*() const {
        if (pos >= order.size()) throw std::out_of_range("MiddleOutOrder dereference out of bounds");
        return order[pos];
    }

    MiddleOutOrder& operator++() { ++pos; return *this; }
    bool operator==(const MiddleOutOrder& other) const { return pos == other.pos; }
    bool operator!=(const MiddleOutOrder& other) const { return !(*this == other); }
    MiddleOutOrder begin() const { return *this; }
    MiddleOutOrder end() const { MiddleOutOrder it = *this; it.pos = order.size(); return it; }
};



// ========================== ITERATOR ACCESSORS ==========================
//
// These functions return the iterators associated with the container,
// allowing clean usage in range-based for loops like:
// for (auto x : container.ascending_order()) { ... }
//

template<typename T>
typename MyContainer<T>::AscendingOrder MyContainer<T>::ascending_order() const {
    return AscendingOrder(*this);
}

template<typename T>
typename MyContainer<T>::DescendingOrder MyContainer<T>::descending_order() const {
    return DescendingOrder(*this);
}

template<typename T>
typename MyContainer<T>::SideCrossOrder MyContainer<T>::sidecross_order() const {
    return SideCrossOrder(*this);
}

template<typename T>
typename MyContainer<T>::ReverseOrder MyContainer<T>::reverse_order() const {
    return ReverseOrder(*this);
}

template<typename T>
typename MyContainer<T>::Order MyContainer<T>::order() const {
    return Order(*this);
}

template<typename T>
typename MyContainer<T>::MiddleOutOrder MyContainer<T>::middle_out_order() const {
    return MiddleOutOrder(*this);
}

} // namespace myns
